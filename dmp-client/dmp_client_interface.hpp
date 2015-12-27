#pragma once

#include <memory>
#include <string>
#include <vector>
#include <stdint.h>

class PlaylistsModel;
class RadioListModel;
class SearchBarModel;
class SearchResultModel;

namespace dmp_library { struct LibraryEntry; }

struct DmpClientUiDelegate;

struct DmpClientInterface {

	virtual void search(std::string str) = 0;
	virtual void add_radio(std::string str) = 0;
	virtual void remove_radio(std::string str) = 0;
	virtual void clear_model() = 0;
	virtual void destroy() = 0;
	virtual void stop() = 0;
	virtual void run() = 0;
	virtual void queue(std::string radio, std::string owner, uint32_t folder_id, dmp_library::LibraryEntry entry) = 0;
	virtual void unqueue(std::string radio, std::vector<uint32_t> ids) = 0;
	virtual void set_current_radio(std::string name) = 0;
	virtual void tune_in(std::string radio, bool tune_in) = 0;
	virtual std::string get_tuned_in_radio() = 0;
	virtual void send_bye() = 0;
	virtual void send_login(std::string username, std::string password) = 0;
	virtual void register_user(std::string username, std::string password) = 0;
	virtual void init_library() = 0;
	virtual void move_queue(std::string radio, std::vector<uint32_t> playlist_id, bool up) = 0;

	virtual void stop_radio() = 0;
	virtual void pause_radio() = 0;
	virtual void play_radio() = 0;
	virtual void next_radio() = 0;
	virtual void mute_radio(bool state) = 0;
	virtual void change_volume(int volume) = 0;
	
	virtual void gstreamer_debug(std::string reason) = 0;

	virtual void add_delegate(std::weak_ptr<DmpClientUiDelegate>) = 0;
	virtual std::shared_ptr<PlaylistsModel> get_playlists_model() = 0;
	virtual std::shared_ptr<RadioListModel> get_radio_list_model() = 0;
	virtual std::shared_ptr<SearchBarModel> get_search_bar_model() = 0;
	virtual std::shared_ptr<SearchResultModel> get_search_result_model() = 0;
	
	virtual ~DmpClientInterface(){}
};
