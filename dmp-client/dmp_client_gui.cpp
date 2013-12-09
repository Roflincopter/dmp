#include "dmp_client_gui.hpp"

#include "ui_dmp_client_gui.hpp"
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

    ui->search_result->setModel(&search_result_model);
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
}


void DmpClientGui::searchBarReturned()
{
    std::string query = ui->search_bar->text().toStdString();
    client->get_callbacks().set(message::Type::SearchResponse, std::function<void(message::SearchResponse)>(std::bind(&SearchResultModel::handle_search_response, &search_result_model, query, std::placeholders::_1)));

    client->search(query);
}

void DmpClientGui::query_parse_error(std::string str) const
{
    QMessageBox popup;
    popup.setModal(true);
    popup.setWindowTitle("Query parse error");
    popup.setText(QString::fromStdString(str));
    popup.exec();
}

void DmpClientGui::indexFolder()
{
    std::string folder = QFileDialog::getExistingDirectory(this, tr("Select music folder to index")).toStdString();
    if(folder.empty()) {
       return;
    }
    client->index(folder);
}

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

DmpClientGui::~DmpClientGui()
{
    delete ui;
}
