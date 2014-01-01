#include "dmp_client_gui_playlists.hpp"

#include <QHeaderView>

DmpClientGuiPlaylists::DmpClientGuiPlaylists(QWidget *parent) : QTableView(parent)
{
	setModel(&model);
	hideColumn(1);
	hideColumn(3);
	hideColumn(5);
	horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void DmpClientGuiPlaylists::set_client(std::shared_ptr<DmpClientInterface> new_client)
{
	client = new_client;
}

void DmpClientGuiPlaylists::playlist_updated(message::PlaylistUpdate update)
{
	model.update_playlist(update.radio_name, update.playlist);
}

void DmpClientGuiPlaylists::currentRadioChanged(std::string name)
{
	model.set_current_radio(name);
}
