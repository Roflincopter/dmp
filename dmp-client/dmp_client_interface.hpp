#pragma once

#include "message_callbacks.hpp"
#include "dmp_client_ui_delegate.hpp"
#include "playlists_model.hpp"
#include "radio_list_model.hpp"

#include <memory>

struct DmpClientInterface {

	virtual void search(std::string str) = 0;
	virtual void index(std::string str) = 0;
	virtual void add_radio(std::string str) = 0;
	virtual void stop() = 0;
	virtual void run() = 0;
	virtual void queue(std::string radio, std::string owner, dmp_library::LibraryEntry entry) = 0;

	virtual void stop_radio(std::string) = 0;
	virtual void pause_radio(std::string) = 0;
	virtual void play_radio(std::string) = 0;
	virtual void next_radio(std::string) = 0;

	virtual void add_delegate(std::weak_ptr<DmpClientUiDelegate> delegate) = 0;
	virtual std::shared_ptr<PlaylistsModel> get_playlists_model() = 0;
	virtual std::shared_ptr<RadioListModel> get_radio_list_model() = 0;

	virtual ~DmpClientInterface(){}
};
