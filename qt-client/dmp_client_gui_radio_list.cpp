#include "dmp_client_gui_radio_list.hpp"

#include "message_outputter.hpp"

#include <QInputDialog>

DmpClientGuiRadioList::DmpClientGuiRadioList(QWidget *parent)
: QListView(parent)
, client(nullptr)
, model()
{
	setModel(&model);
}

void DmpClientGuiRadioList::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected)
{
	emit currentlySelectedRadio(model.data(selected.indexes()[0], Qt::DisplayRole).toString().toStdString());
	QListView::selectionChanged(selected, deselected);
}

void DmpClientGuiRadioList::set_client(std::shared_ptr<DmpClientInterface> new_client)
{
	client = new_client;
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

void DmpClientGuiRadioList::radios_update(message::Radios radios)
{
	std::vector<std::string> radio_names;
	for(auto& radio :radios.radios)
	{
		radio_names.push_back(radio.first);
	}
	model.set_radio_names(radio_names);
}

void DmpClientGuiRadioList::radio_added(message::AddRadio added_radio)
{
	model.add_radio(added_radio.name);
}
