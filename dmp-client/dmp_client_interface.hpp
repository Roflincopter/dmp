#pragma once

#include "message_callbacks.hpp"
#include "dmp_client_ui_delegate.hpp"
#include "playlists_model.hpp"
#include "radio_list_model.hpp"
#include "search_bar_model.hpp"
#include "search_result_model.hpp"

#include <memory>

struct DmpClientInterface {

	virtual void search(std::string str) = 0;
	virtual void index(std::string str) = 0;
	virtual void add_radio(std::string str) = 0;
	virtual void remove_radio(std::string str) = 0;
	virtual void destroy() = 0;
	virtual void stop() = 0;
	virtual void run() = 0;
	virtual void queue(std::string radio, std::string owner, dmp_library::LibraryEntry entry) = 0;
	virtual void set_current_radio(std::string name) = 0;
	virtual void tune_in(std::string radio, bool tune_in) = 0;
	virtual std::string get_tuned_in_radio() = 0;
	virtual void send_bye() = 0;
	virtual void send_login(std::string username, std::string password) = 0;
	virtual void register_user(std::string username, std::string password) = 0;

	virtual void stop_radio() = 0;
	virtual void pause_radio() = 0;
	virtual void play_radio() = 0;
	virtual void next_radio() = 0;
	virtual void mute_radio(bool state) = 0;

	virtual void add_delegate(std::weak_ptr<DmpClientUiDelegate>) = 0;
	virtual std::shared_ptr<PlaylistsModel> get_playlists_model() = 0;
	virtual std::shared_ptr<RadioListModel> get_radio_list_model() = 0;
	virtual std::shared_ptr<SearchBarModel> get_search_bar_model() = 0;
	virtual std::shared_ptr<SearchResultModel> get_search_result_model() = 0;
	
	virtual ~DmpClientInterface(){}
};
