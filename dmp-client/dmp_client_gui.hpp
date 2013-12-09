#pragma once

#include "search_result_model.hpp"
#include "dmp_client.hpp"
#include "dmp_client_interface.hpp"

#include <QMainWindow>
#include <QStandardItemModel>

#include <thread>
#include <memory>

namespace Ui {
class DmpClientGui;
}

class DmpClientGui : public QMainWindow, public UiInterface, public std::enable_shared_from_this<UiInterface>
{
    Q_OBJECT

    SearchResultModel search_result_model;
    std::shared_ptr<DmpClientInterface> client;
    std::thread client_thread;

public:
    explicit DmpClientGui(QWidget *parent = 0);
    ~DmpClientGui();

    void set_client(std::shared_ptr<DmpClientInterface> new_client);

    void query_parse_error(std::string err) const final;
    void closeEvent(QCloseEvent*) final;

public slots:
    void searchBarReturned();
    void indexFolder();

private:
    Ui::DmpClientGui *ui;
};

