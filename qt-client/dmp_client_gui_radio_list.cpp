#include "dmp_client_gui_radio_list.hpp"

#include "message_outputter.hpp"

#include <QInputDialog>

DmpClientGuiRadioList::DmpClientGuiRadioList(QWidget *parent)
: QListView(parent)
, client(nullptr)
, model(std::make_shared<RadioListModelQtAdapter>())
{
	setModel(model.get());
}

void DmpClientGuiRadioList::selectionChanged(QItemSelection const& selected, QItemSelection const& deselected)
{
	emit currentlySelectedRadio(model->data(selected.indexes()[0], Qt::DisplayRole).toString().toStdString());
	QListView::selectionChanged(selected, deselected);
}

void DmpClientGuiRadioList::set_client(std::shared_ptr<DmpClientInterface> new_client)
{
	client = new_client;
	model->set_model(client->get_radio_list_model());
	client->add_delegate(model);
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