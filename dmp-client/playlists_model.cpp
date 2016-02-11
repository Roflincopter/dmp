#include "playlists_model.hpp"

#include "message.hpp"

#include "playlist_ui_delegate.hpp"

#include "fusion_static_dispatch.hpp"

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <vector>

namespace dmp_library { struct LibraryEntry; }

PlaylistsModel::PlaylistsModel()
{
}

std::string PlaylistsModel::get_current_radio() const
{
	return current_radio;
}

int PlaylistsModel::row_count() const
{
	auto it = playlists.find(current_radio);
	if(it != playlists.end()) {
		return it->second.size();
	} else {
		return 0;
	}
}

int PlaylistsModel::column_count() const
{
	return number_of_library_entry_members + number_of_queuer_owner_pair_members;
}

std::string PlaylistsModel::header_data(int section) const
{	
	if(section < 0 || section >= column_count()) {
		throw std::out_of_range("Column index was out of range.");
	}
	
	return get_nth_name<boost::fusion::joint_view<dmp_library::LibraryEntry, PlaylistEntry>>(section);
}

boost::any PlaylistsModel::get_cell(int row, int column) const
{
	auto it = playlists.find(current_radio);
	if(it == playlists.end()) {
		std::runtime_error("internally selected current radio does not exist.");
	}

	Playlist const& datalist = it->second;

	if(row < 0 || size_t(row) >= datalist.size()) {
		throw std::out_of_range("Row index was out of range.");
	}

	if(column < 0 || column >= column_count()) {
		throw std::out_of_range("Column index was out of range.");
	}

	auto data = datalist[row];
	PlaylistEntry::QueuerOwnerPair p{data.queuer, data.owner};
	return get_nth(ElementType(data.entry, p), column);
}

void PlaylistsModel::set_cell(boost::any const& value, int row, int column)
{
	auto it = playlists.find(current_radio);
	if(it == playlists.end()) {
		std::runtime_error("internally selected current radio does not exist.");
	}

	Playlist& datalist = it->second;

	if(row < 0 || size_t(row) >= datalist.size()) {
		throw std::out_of_range("Row index was out of range.");
	}

	if(column < 0 || column >= column_count()) {
		throw std::out_of_range("Column index was out of range.");
	}
	auto& data = datalist[row];
	PlaylistEntry::QueuerOwnerPair p{data.queuer, data.owner};
	set_nth(ElementType(data.entry, p), column, value);
}

uint32_t PlaylistsModel::get_playlist_id(uint32_t row) const
{
	auto it = playlists.find(current_radio);
	if(it == playlists.end()) {
		std::runtime_error("internally selected current radio does not exist.");
	}

	Playlist const& datalist = it->second;

	if(size_t(row) >= datalist.size()) {
		throw std::out_of_range("Row index was out of range.");
	}

	auto& data = datalist[row];
	return data.playlist_id;
}

void PlaylistsModel::update(std::string radio_name, Playlist playlist)
{
	call_on_delegates(&PlaylistUiDelegate::set_radios_start);
	std::swap(playlists[radio_name], playlist);
	call_on_delegates(&PlaylistUiDelegate::set_radios_end);
	
}

void PlaylistsModel::remove(std::string radio_name, std::vector<uint32_t> ids)
{
	auto it = playlists.find(radio_name);
	if(it == playlists.end()) {
		throw std::runtime_error("The radio on which append was called does not exists " + radio_name);
	}
	
	auto&& playlist = it->second;
	
	for(auto&& id : ids) {
		auto erase_it = playlist.end();
		for(auto it = playlist.begin(); it != playlist.end(); ++it) {
			if(it->playlist_id == id) {
				erase_it = it;
				break;
			}
		}
		if(erase_it != playlist.end()) {
			playlist.erase(erase_it);
		}
	}
}

void PlaylistsModel::append(std::string radio_name, Playlist playlist)
{
	auto it = playlists.find(radio_name);
	if(it == playlists.end()) {
		throw std::runtime_error("The radio on which append was called does not exists " + radio_name);
	}
	
	playlist.reserve(playlists[radio_name].size() + playlist.size());
	std::copy(playlist.begin(), playlist.end(), std::back_inserter(playlists[radio_name]));
}

void PlaylistsModel::reset(std::string radio_name)
{
	auto it = playlists.find(radio_name);
	if(it == playlists.end()) {
		throw std::runtime_error("The radio on which reset was called does not exists " + radio_name);
	}
	
	playlists[radio_name].clear();
}

void PlaylistsModel::clear()
{
	call_on_delegates(&PlaylistUiDelegate::clear_start);
	playlists.clear();
	call_on_delegates(&PlaylistUiDelegate::clear_end);
}

bool PlaylistsModel::handle_update(message::PlaylistUpdate update_msg)
{
	call_on_delegates(&PlaylistUiDelegate::playlist_update_start, update_msg);
	switch(update_msg.action.type)
	{
		case message::PlaylistUpdate::Action::Type::Append:
		{
			append(update_msg.radio_name, update_msg.playlist);
			break;
		}
		case message::PlaylistUpdate::Action::Type::Update:
		{
			update(update_msg.radio_name, update_msg.playlist);
			break;
		}
		case message::PlaylistUpdate::Action::Type::Insert:
		{
			break;
		}
		case message::PlaylistUpdate::Action::Type::Remove:
		{
			remove(update_msg.radio_name, update_msg.action.ids);
			break;
		}
		case message::PlaylistUpdate::Action::Type::MoveUp:
		{
			break;
		}
		case message::PlaylistUpdate::Action::Type::MoveDown:
		{
			break;
		}
		case message::PlaylistUpdate::Action::Type::Reset:
		{
			reset(update_msg.radio_name);
			break;
		}
		case message::PlaylistUpdate::Action::Type::NoAction:
		default:
		{
			throw std::runtime_error("This should never happen");
		}
	}
	call_on_delegates(&PlaylistUiDelegate::playlist_update_end, update_msg);
	return true;
}

void PlaylistsModel::set_current_radio(std::string radio_name)
{
	call_on_delegates(&PlaylistUiDelegate::current_radio_change_start);
	current_radio = radio_name;
	call_on_delegates(&PlaylistUiDelegate::current_radio_change_end);
}

void PlaylistsModel::create_radio(std::string radio_name)
{
	call_on_delegates(&PlaylistUiDelegate::add_radio_start);
	playlists[radio_name] = {};
	call_on_delegates(&PlaylistUiDelegate::add_radio_end);
}

void PlaylistsModel::remove_radio(std::string radio_name)
{
	call_on_delegates(&PlaylistUiDelegate::remove_radio_start);
	playlists.erase(radio_name);
	call_on_delegates(&PlaylistUiDelegate::remove_radio_end);
}
