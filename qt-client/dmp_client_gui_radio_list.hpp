#pragma once

#include "dmp_client_ui_delegate.hpp"
#include "radio_list_ui_delegate.hpp"
#include "dmp_client_ui_controller_interface.hpp"
#include "radio_list_model_qt_adapter.hpp"

#include <QListView>


class DmpClientGuiRadioList
: public QListView
, public DmpClientUiControllerInterface
, public RadioListUiDelegate
, public std::enable_shared_from_this<RadioListUiDelegate>
{
	Q_OBJECT

	std::shared_ptr<DmpClientInterface> client;
	std::shared_ptr<RadioListModelQtAdapter> model;

	std::string current_selected_radio;

public:
	explicit DmpClientGuiRadioList(QWidget *parent = 0);

	virtual void selectionChanged(QItemSelection const& selected, QItemSelection const& deselected) override final;

	virtual void set_client(std::shared_ptr<DmpClientInterface> new_client) override final;
	
	virtual void set_radio_states() override final;
	
	void set_selection(int row);
	
signals:
	void setTuneInChecked(bool);
	void setPlayingChecked(bool);
	void currentlySelectedRadio(std::string);

public slots:
	void addRadio();
	void tuneIn(bool state);
};
