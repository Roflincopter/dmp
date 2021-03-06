#pragma once

#include "playlist.hpp"
#include "friendly_fusion.hpp"
#include "delegator.hpp"

#include <boost/any.hpp>
#include <boost/fusion/view/joint_view/joint_view.hpp>
#include <boost/mpl/int.hpp>

#include <stddef.h>
#include <stdint.h>
#include <map>
#include <string>

namespace dmp_library { struct LibraryEntry; }

namespace message { struct PlaylistUpdate; }

struct PlaylistUiDelegate;

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

	uint32_t get_playlist_id(uint32_t row) const;

	void update(std::string radio_name, Playlist playlist);
	void remove(std::string radio_name, std::vector<uint32_t> ids);
	void append(std::string radio_name, Playlist playlist);
	void reset(std::string radio_name);
	void clear();
	
	bool handle_update(message::PlaylistUpdate update_msg);

	void set_current_radio(std::string radio_name);
	void create_radio(std::string radio_name);
	void remove_radio(std::string radio_name);
};
