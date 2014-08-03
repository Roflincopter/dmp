#include "playlists_model.hpp"

#include "fusion_static_dispatch.hpp"
#include "fusion_outputter.hpp"

#include <boost/mpl/joint_view.hpp>
#include <boost/fusion/sequence.hpp>

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
	return number_of_library_entry_members + number_of_playlist_entry_members;
}

std::string PlaylistsModel::header_data(int section) const
{	
	if(section < 0 || size_t(section) >= number_of_library_entry_members + number_of_playlist_entry_members) {
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

	if(column < 0 || size_t(column) >= number_of_library_entry_members + number_of_playlist_entry_members) {
		throw std::out_of_range("Column index was out of range.");
	}

	auto data = datalist[row];
	return get_nth(boost::fusion::joint_view<dmp_library::LibraryEntry, PlaylistEntry>(data.entry, data), column);
}

void PlaylistsModel::update(std::string radio_name, Playlist playlist)
{
	playlists[radio_name] = playlist;
}

void PlaylistsModel::append(std::string radio_name, Playlist playlist)
{
	auto it = playlists.find(radio_name);
	if(it == playlists.end()) {
		throw std::runtime_error("The radio on which append was called does not exists " + radio_name);
	}

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

void PlaylistsModel::handle_update(message::PlaylistUpdate update_msg)
{
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
		case message::PlaylistUpdate::Action::Type::Move:
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
}

void PlaylistsModel::set_current_radio(std::string radio_name)
{
	current_radio = radio_name;
}

void PlaylistsModel::create_radio(std::string radio_name)
{
	playlists[radio_name] = {};
}
