#pragma once

#include "dmp_client.hpp"
#include "search_result_model.hpp"

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

    SearchResultModel search_result_model;

public:
    explicit DmpClientGui(DmpClient& client_ref, QWidget *parent = 0);
    ~DmpClientGui();

public slots:
    void searchBarReturned();

    void indexFolder();

private:
    Ui::DmpClientGui *ui;
};

