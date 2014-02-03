#include "playlists_model.hpp"

#include "fusion_static_dispatch.hpp"
#include "message_outputter.hpp"

PlaylistsModel::PlaylistsModel()
{
}

int PlaylistsModel::row_count() const
{
	if(current_radio.empty()) {
		return 0;
	}

	auto it = playlists.find(current_radio);
	if(it != playlists.end()) {
		return it->second.size();
	} else {
		return 0;
	}
}

int PlaylistsModel::column_count() const
{
	return boost::fusion::result_of::size<dmp_library::LibraryEntry>::type::value + 2;
}

std::string PlaylistsModel::header_data(int section) const
{	
	if(section < 0 || size_t(section) >= number_of_library_entry_members + number_of_playlist_entry_members) {
		throw std::out_of_range("Column index was out of range.");
	}
	
	size_t index = section;
	
	//return get_nth_name<boost::fusion::joint_view<dmp_library::LibraryEntry, PlaylistEntry>::concat_type> (size_t(section));
	
	if(index < number_of_library_entry_members) {
		return get_nth_name<dmp_library::LibraryEntry>(index);
	} else {
		index -= number_of_library_entry_members;
		return get_nth_name<PlaylistEntry>(index);
	}
}

std::string PlaylistsModel::get_cell(int row, int column) const
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
	
	size_t index = size_t(column);
	if(index < number_of_library_entry_members) {
		return get_nth(data.entry, index);
	} else {
		index -= number_of_library_entry_members;
		return get_nth(data, index);
	}
}

void PlaylistsModel::update_playlist(std::string radio_name, Playlist playlist)
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

void PlaylistsModel::set_current_radio(std::string radio_name)
{
	current_radio = radio_name;
}

void PlaylistsModel::create_radio(std::string radio_name)
{
	playlists[radio_name] = {};
}
