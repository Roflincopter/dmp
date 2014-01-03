#include "dmp_client_gui.hpp"

#include "message_outputter.hpp"
#include "connect.hpp"
#include "dmp_client_connect_dialog.hpp"

#include <QFileDialog>
#include <QMessageBox>
#include <QKeySequence>

#include <iostream>
#include <string>

DmpClientGui::DmpClientGui(QWidget *parent)
: QMainWindow(parent)
, search_result_model()
, client(nullptr)
, client_thread()
, test1_key(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_A), this, SLOT(test1()))
, shared_main_window(nullptr)
, shared_menu_bar(nullptr)
, shared_search_bar(nullptr)
, shared_search_results(nullptr)
, shared_radio_list(nullptr)
, shared_playlists(nullptr)
, ui()
, client_synchronisation_thread()
{
	ui.setupUi(this);

	shared_main_window = std::shared_ptr<DmpClientGui>(this, [](void*){});
	shared_menu_bar = std::shared_ptr<DmpClientGuiMenuBar>(ui.menu_bar, [](void*){});
	shared_search_bar = std::shared_ptr<DmpClientGuiSearchBar>(ui.search_bar, [](void*){});
	shared_search_results = std::shared_ptr<DmpClientGuiSearchResults>(ui.search_results, [](void*){});
	shared_radio_list = std::shared_ptr<DmpClientGuiRadioList>(ui.radio_list, [](void*){});
	shared_playlists = std::shared_ptr<DmpClientGuiPlaylists>(ui.playlists, [](void*){});

	test1_key.setAutoRepeat(false);
	test1_key.setContext(Qt::ApplicationShortcut);
}

void DmpClientGui::update_ui_client_interface()
{
	client->add_delegate(shared_main_window);

	shared_menu_bar->set_client(client);
	client->add_delegate(shared_menu_bar);
	shared_search_bar->set_client(client);
	client->add_delegate(shared_search_bar);
	shared_search_results->set_client(client);
	client->add_delegate(shared_search_results);
	shared_radio_list->set_client(client);
	client->add_delegate(shared_radio_list);
	shared_playlists->set_client(client);
	client->add_delegate(shared_playlists);
}

void DmpClientGui::test1()
{
	connect_client(boost::asio::ip::host_name(), "localhost", 1337);
	client->index("/home/dennis/Music");
	client->add_radio("Radio1");
	client->search("artist contains \"C\"");
}

void DmpClientGui::set_client(std::shared_ptr<DmpClientInterface> new_client)
{
	if(!client) {
		client = new_client;

	} else {
		if(client_thread.joinable()) {
			client_thread.join();

			client = new_client;
		}
		else {
			throw std::runtime_error("Tried to assign a new client when old client was still running.");
		}
	}

	auto client_runner = [this]
	{
		try {
			client->run();
		} catch (std::runtime_error e) {
			std::cerr << "Connection to server lost with message: " << e.what() << std::endl;
		}
	};

	client_thread = std::thread(client_runner);

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

void DmpClientGui::connect_client(std::string name, std::string host, uint16_t port)
{
	auto client_sp = std::make_shared<DmpClient>(name, host, port);

	set_client(client_sp);

	ui.actionIndex_Folder->setEnabled(true);
	ui.search_bar->setEnabled(true);
	ui.addRadioButton->setEnabled(true);
	ui.deleteRadioButton->setEnabled(true);
}

void DmpClientGui::dmpConnect()
{
	DmpClientConnectDialog connect;

	connect.set_default_name(boost::asio::ip::host_name());

	int result = connect.exec();

	if(result != QDialog::Accepted) {
		return;
	}

	connect_client(connect.get_name(), connect.get_host(), connect.get_port());
}

void DmpClientGui::closeEvent(QCloseEvent*)
{
	if(!client) {
		return;
	}

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
