#pragma once

#include "dmp_client.hpp"

#include <QMainWindow>
#include <QStandardItemModel>

#include <thread>

namespace Ui {
class DmpClientGui;
}

class DmpClientGui : public QMainWindow
{
    Q_OBJECT

    DmpClient& client;
    std::thread client_thread;

    QStandardItemModel search_response_model;

public:
    explicit DmpClientGui(DmpClient& client_ref, QWidget *parent = 0);
    ~DmpClientGui();

public slots:
    void searchBarReturned();

    void indexFolder();

private:
    Ui::DmpClientGui *ui;

    void handle_search_response(std::string query, message::SearchResponse);
};

