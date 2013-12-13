#pragma once

#include "dmp_client_ui_controller_interface.hpp"

#include <QLineEdit>

class DmpClientGuiSearchBar : public QLineEdit, public DmpClientUiControllerInterface, public DmpClientUiDelegate
{
    Q_OBJECT

    std::shared_ptr<DmpClientInterface> client;
public:
    explicit DmpClientGuiSearchBar(QWidget *parent = 0);

    virtual void set_client(std::shared_ptr<DmpClientInterface> new_client) final;
signals:

public slots:

};
