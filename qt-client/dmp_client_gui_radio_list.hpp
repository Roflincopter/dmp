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

	virtual void selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) override final;

	virtual void set_client(std::shared_ptr<DmpClientInterface> new_client) override final;

	virtual void radios_update(message::Radios) override final;
	virtual void radio_added(message::AddRadio radio_added) override final;

	//TODO: remove this ugly hack. The models should reside in the client just as the seperate views reside in the mainwindow.
	//This allows the model to be accessable in the client and the gui only needs to ask to stop/pause/next the current radio.
	std::string get_current_radio()
	{
		return model.data(selectedIndexes()[0], Qt::DisplayRole).toString().toStdString();
	}
	
signals:
	void currentlySelectedRadio(std::string);

public slots:
	void addRadio();

};
