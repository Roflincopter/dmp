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
	switch(update.action.type)
	{
		case message::PlaylistUpdate::Action::Type::Append:
		{
			model.append(update.radio_name, update.playlist);
			break;
		}
		case message::PlaylistUpdate::Action::Type::Insert:
		{
			break;
		}
		case message::PlaylistUpdate::Action::Type::Move:
		{
			break;
		}
		case message::PlaylistUpdate::Action::Type::Reset:
		{
			break;
		}
		case message::PlaylistUpdate::Action::Type::NoAction:
		default:
		{
			throw std::runtime_error("This should never happen");
		}
	}

	model.update_playlist(update.radio_name, update.playlist);
}

void DmpClientGuiPlaylists::add_radio_succes(message::AddRadioResponse response)
{
	model.create_radio(response.radio_name);
}

void DmpClientGuiPlaylists::currentRadioChanged(std::string name)
{
	model.set_current_radio(name);
}
