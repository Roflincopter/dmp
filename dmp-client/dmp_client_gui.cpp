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
, ui(new Ui::DmpClientGui)
{
    ui->setupUi(this);
}

void DmpClientGui::update_ui_client_interface()
{
    shared_menu_bar = std::shared_ptr<DmpClientGuiMenuBar>(ui->menu_bar, [](void*){});
    shared_search_bar = std::shared_ptr<DmpClientGuiSearchBar>(ui->search_bar, [](void*){});
    shared_search_results = std::shared_ptr<DmpClientGuiSearchResults>(ui->search_results, [](void*){});

    ui->menu_bar->set_client(client);
    client->add_delegate(shared_menu_bar);
    ui->search_bar->set_client(client);
    client->add_delegate(shared_search_bar);
    ui->search_results->set_client(client);
    client->add_delegate(shared_search_results);
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


void DmpClientGui::searchBarReturned()
{
    std::string query = ui->search_bar->text().toStdString();
    client->search(query);
}
/*
void DmpClientGui::query_parse_error(std::string str) const
{
    QMessageBox popup;
    popup.setModal(true);
    popup.setWindowTitle("Query parse error");
    popup.setText(QString::fromStdString(str));
    popup.exec();
}
*/

void DmpClientGui::closeEvent(QCloseEvent*)
{
    //Shutdown of the client and client GUI.
    auto shutdown = [this]{
        client->stop();
    };

    //Create a thread to synchronise with the server to reach a destructable state.
    std::thread t;

    //Create a callback to shut down the client.
    auto cb = [&t, this, shutdown](message::ByeAck){
        t = std::thread(shutdown);
    };

    //Register a callback to do the destruction as soon as we receive the ByeAck.
    client->get_callbacks().set(message::Type::ByeAck, cb);
    client->send_bye();

    while (!t.joinable()) {
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    //Join the destructor thread.
    t.join();

    //join the client_thread
    client_thread.join();
}
