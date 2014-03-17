#include "dmp_client_gui_playlists.hpp"

#include <QHeaderView>

DmpClientGuiPlaylists::DmpClientGuiPlaylists(QWidget *parent) : QTableView(parent)
{
	setModel(&model);
	setItemDelegate(&delegate);
	hideColumn(1);
	hideColumn(3);
	hideColumn(5);
	horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void DmpClientGuiPlaylists::set_client(std::shared_ptr<DmpClientInterface> new_client)
{
	client = new_client;
}

void DmpClientGuiPlaylists::radios_update(message::Radios update)
{
	for(auto&& radio : update.radios)
	{
		model.update(radio.first, radio.second);
	}
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
		case message::PlaylistUpdate::Action::Type::Update:
		{
			model.update(update.radio_name, update.playlist);
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
			model.reset(update.radio_name);
			break;
		}
		case message::PlaylistUpdate::Action::Type::NoAction:
		default:
		{
			throw std::runtime_error("This should never happen");
		}
	}
}

void DmpClientGuiPlaylists::add_radio_succes(message::AddRadioResponse response)
{
	model.create_radio(response.radio_name);
}

void DmpClientGuiPlaylists::currentRadioChanged(std::string name)
{
	model.set_current_radio(name);
}
