#include "dmp_client_gui_radio_list.hpp"

#include "debug_macros.hpp"

#include <QInputDialog>
#include <QPushButton>

DmpClientGuiRadioList::DmpClientGuiRadioList(QWidget *parent)
: QListView(parent)
, client(nullptr)
, model(std::make_shared<RadioListModelQtAdapter>())
{
	setModel(model.get());
}

void DmpClientGuiRadioList::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected)
{
	auto variant = model->data(selected.indexes()[0], Qt::DisplayRole);
	if(variant.isValid()) {
		current_selected_radio = variant.toString().toStdString();
		emit setTuneInChecked(current_selected_radio == client->get_tuned_in_radio());
		emit currentlySelectedRadio(current_selected_radio);
		client->set_current_radio(current_selected_radio);
	}
	QListView::selectionChanged(selected, deselected);
}

void DmpClientGuiRadioList::set_client(std::shared_ptr<DmpClientInterface> new_client)
{
	client = new_client;
	model->set_model(client->get_radio_list_model());
	client->get_radio_list_model()->add_delegate<RadioListUiDelegate>(shared_from_this());
}

void DmpClientGuiRadioList::set_radio_states()
{
	if(current_selected_radio == "" && model->model->row_count() != 0) {
		set_selection(0);
	}
	emit setPlayingChecked(model->model->get_radio_states()[current_selected_radio].playing);
}

void DmpClientGuiRadioList::add_radio_end()
{
	if(selectionModel()->selectedRows().count() == 1) {
		set_selection(0);
	}
}

void DmpClientGuiRadioList::set_selection(int row)
{
	auto index = model->get_model_index_for(row);
	if(index.isValid()) {
		selectionModel()->select(index, QItemSelectionModel::Select);
		selectionChanged(selectionModel()->selection(), selectionModel()->selection());
	}
}

void DmpClientGuiRadioList::addRadio()
{
	QInputDialog dialog;
	dialog.setLabelText("Name your Radio.");
	int ret = dialog.exec();

	if(ret == dialog.Rejected) {
		return;
	}

	std::string radio_name = dialog.textValue().toStdString();
	client->add_radio(radio_name);
}

void DmpClientGuiRadioList::removeRadio()
{
	client->remove_radio(current_selected_radio);
}

void DmpClientGuiRadioList::tuneIn(bool state)
{
	std::string current_tuned_in_radio = model->model->get_tuned_in_radio();

	if(state && !current_tuned_in_radio.empty()) {
		client->tune_in(current_tuned_in_radio, false);
	}
	client->tune_in(current_selected_radio, state);
}
