#include "dmp_client_gui.hpp"

#include "message_outputter.hpp"
#include "connect.hpp"

#include <QFileDialog>
#include <QMessageBox>

#include <iostream>
#include <string>
#include <condition_variable>

DmpClientGui::DmpClientGui(QWidget *parent)
: QMainWindow(parent)
, search_result_model()
, client(nullptr)
, client_thread()
, shared_main_window(nullptr)
, shared_menu_bar(nullptr)
, shared_search_bar(nullptr)
, shared_search_results(nullptr)
, shared_radio_list(nullptr)
, ui(new Ui::DmpClientGui)
, client_synchronisation_thread()
{
	ui->setupUi(this);
}

void DmpClientGui::update_ui_client_interface()
{
	shared_main_window = std::shared_ptr<DmpClientGui>(this, [](void*){});
	shared_menu_bar = std::shared_ptr<DmpClientGuiMenuBar>(ui->menu_bar, [](void*){});
	shared_search_bar = std::shared_ptr<DmpClientGuiSearchBar>(ui->search_bar, [](void*){});
	shared_search_results = std::shared_ptr<DmpClientGuiSearchResults>(ui->search_results, [](void*){});
	shared_radio_list = std::shared_ptr<DmpClientGuiRadioList>(ui->radio_list, [](void*){});

	shared_menu_bar->set_client(client);
	client->add_delegate(shared_menu_bar);
	shared_search_bar->set_client(client);
	client->add_delegate(shared_search_bar);
	shared_search_results->set_client(client);
	client->add_delegate(shared_search_results);
	shared_radio_list->set_client(client);
	client->add_delegate(shared_radio_list);

	client->add_delegate(shared_main_window);
}

void DmpClientGui::set_client(std::shared_ptr<DmpClientInterface> new_client)
{
	if(!client) {
		client = new_client;

		client_thread = std::thread(std::bind(&DmpClientInterface::run, client));
	} else {
		if(client_thread.joinable()) {
			client_thread.join();

			client = new_client;

			client_thread = std::thread(std::bind(&DmpClientInterface::run, client));
		}
		else {
			throw std::runtime_error("Tried to assign a new client when old client was still running.");
		}
	}
	update_ui_client_interface();
}

void DmpClientGui::bye_ack_received()
{
	//Shutdown of the client.
	auto shutdown = [this]{
		client->stop();
	};

	//initiate shutdown
	client_synchronisation_thread = std::thread(shutdown);
}

void DmpClientGui::closeEvent(QCloseEvent*)
{
	//Send the bye and make sure we override the delegate function bye_ack_received.
	client->send_bye();

	while (!client_synchronisation_thread.joinable()) {
		std::this_thread::sleep_for(std::chrono::microseconds(1));
	}

	//Join the destructor thread.
	client_synchronisation_thread.join();

	//join the client_thread
	client_thread.join();
}
