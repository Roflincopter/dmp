#pragma once

#include <QListView>

#include "dmp_client_ui_delegate.hpp"
#include "dmp_client_ui_controller_interface.hpp"
#include "radio_list_model_qt_adapter.hpp"

class DmpClientGuiRadioList : public QListView, public DmpClientUiControllerInterface, public DmpClientUiDelegate
{
	Q_OBJECT

	std::shared_ptr<DmpClientInterface> client;
	RadioListModelQtAdapter model;

public:
	explicit DmpClientGuiRadioList(QWidget *parent = 0);

	virtual void set_client(std::shared_ptr<DmpClientInterface> new_client) final;

	virtual void radios_update(message::Radios) final;

signals:

public slots:

	void addRadio();
};