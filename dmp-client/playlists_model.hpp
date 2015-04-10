#pragma once

#include "message.hpp"
#include "playlist.hpp"
#include "friendly_fusion.hpp"
#include "playlist_ui_delegate.hpp"
#include "delegator.hpp"

#include "dmp-library.hpp"

#include <boost/any.hpp>

#include <memory>
#include <vector>

class PlaylistsModel : public Delegator<PlaylistUiDelegate>
{
	static constexpr size_t number_of_library_entry_members = friendly_fusion::result_of::size<dmp_library::LibraryEntry>::type::value;
	static constexpr size_t number_of_queuer_owner_pair_members = friendly_fusion::result_of::size<PlaylistEntry::QueuerOwnerPair>::type::value;
	
	std::map<std::string, Playlist> playlists;
	std::string current_radio;

public:
	PlaylistsModel();

	typedef boost::fusion::joint_view<dmp_library::LibraryEntry, PlaylistEntry::QueuerOwnerPair> ElementType;
	
	std::string get_current_radio() const;
	
	int row_count() const;
	int column_count() const;

	std::string header_data(int section) const;
	boost::any get_cell(int row, int column) const;

	void set_cell(const boost::any &value, int row, int column);

	void update(std::string radio_name, Playlist playlist);
	void append(std::string radio_name, Playlist playlist);
	void reset(std::string radio_name);
	
	bool handle_update(message::PlaylistUpdate update_msg);

	void set_current_radio(std::string radio_name);
	void create_radio(std::string radio_name);
	void remove_radio(std::string radio_name);
};
