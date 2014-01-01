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
	size_t number_of_entry_members = boost::fusion::result_of::size<dmp_library::LibraryEntry>::type::value;
	if(section >= number_of_entry_members + 2 || section < 0) {
		throw std::out_of_range("Column index was out of range.");
	}

	if(section < number_of_entry_members) {
		return get_nth_name<dmp_library::LibraryEntry>(section);
	} else {
		if(section == number_of_entry_members) {
			return "queuer";
		} else if (section == number_of_entry_members + 1) {
			return "owner";
		} else {
			throw std::runtime_error("This should not happen");
		}
	}
}

std::string PlaylistsModel::get_cell(int row, int column) const
{
	size_t number_of_entry_members = boost::fusion::result_of::size<dmp_library::LibraryEntry>::type::value;

	auto it = playlists.find(current_radio);
	if(it == playlists.end()) {
		std::runtime_error("internally selected current radio does not exist.");
	}

	message::Playlist const& datalist = it->second;

	if(row < 0 || row >= datalist.size()) {
		throw std::out_of_range("Row index was out of range.");
	}

	if(column < 0 || column >= number_of_entry_members + 2) {
		throw std::out_of_range("Column index was out of range.");
	}

	auto data = datalist[row];
	if(column == number_of_entry_members) {
		return std::get<0>(data);
	} else if(column == number_of_entry_members + 1) {
		return std::get<1>(data);
	} else {
		return get_nth(std::get<2>(data), column);
	}
}

void PlaylistsModel::update_playlist(std::string radio_name, message::Playlist playlist)
{
	playlists[radio_name] = playlist;
}

void PlaylistsModel::set_current_radio(std::string radio_name)
{
	current_radio = radio_name;
}