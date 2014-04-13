#include "dmp_client_gui_playlists.hpp"

#include <QHeaderView>

DmpClientGuiPlaylists::DmpClientGuiPlaylists(QWidget *parent)
: QTableView(parent)
, client(nullptr)
, model(std::make_shared<PlaylistsModelQtAdapter>())
, delegate()
{
	setModel(model.get());
	setItemDelegate(&delegate);
	hideColumn(1);
	hideColumn(3);
	hideColumn(5);
	horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void DmpClientGuiPlaylists::set_client(std::shared_ptr<DmpClientInterface> new_client)
{
	client = new_client;
	model->set_model(client->get_playlists_model());
	client->add_delegate(model);
}

void DmpClientGuiPlaylists::currentRadioChanged(std::string name)
{
	client->set_current_radio(name);
}
