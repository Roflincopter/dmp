 #pragma once

#include "connection.hpp"
#include "dmp_sender.hpp"
#include "dmp_receiver.hpp"
#include "dmp_client_interface.hpp"
#include "dmp_client_ui_delegate.hpp"
#include "playlists_model.hpp"

#include "message_switch.hpp"

#include <thread>

class DmpClient : public DmpClientInterface
{
	std::string name;
	std::string host;
	message::DmpCallbacks callbacks;
	dmp::Connection connection;
	message::Ping last_sent_ping;
	dmp_library::Library library;

	std::vector<std::weak_ptr<DmpClientUiDelegate>> delegates;
	
	std::map<std::string, DmpSender> senders;
	
	std::thread receiver_thread;
	DmpReceiver receiver;

	MessageSwitch message_switch;
	
	boost::asio::deadline_timer debug_timer;
	
	std::shared_ptr<PlaylistsModel> playlists_model;

public:

	DmpClient(std::string name, std::string host, uint16_t port);
	DmpClient(DmpClient&&) = default;
	virtual ~DmpClient();
	
	void timed_debug();

	message::DmpCallbacks::Callbacks_t initial_callbacks();
	
	void add_delegate(std::weak_ptr<DmpClientUiDelegate> delegate);
	std::shared_ptr<PlaylistsModel> get_playlists_model();

	void handle_request(message::Type t);
	void listen_requests();

	virtual void stop() override final;
	virtual void run() override final;
	virtual void search(std::string query) override final;
	virtual void index(std::string path) override final;
	virtual void add_radio(std::string str) override final;
	virtual void queue(std::string radio, std::string owner, dmp_library::LibraryEntry entry) override final;

	virtual void stop_radio(std::string radio_name) override final;
	virtual void play_radio(std::string radio_name) override final;
	virtual void pause_radio(std::string radio_name) override final;
	virtual void next_radio(std::string radio_name) override final;
	
	void handle_ping(message::Ping ping);
	void handle_name_request(message::NameRequest name_req);
	void handle_pong(message::Pong pong);
	void handle_search_request(message::SearchRequest search_req);
	void handle_search_response(message::SearchResponse search_res);
	void handle_bye_ack(message::ByeAck);
	void handle_add_radio_response(message::AddRadioResponse);
	void handle_listener_connection_request(message::ListenConnectionRequest req);
	void handle_radios(message::Radios radios);
	void handle_add_radio(message::AddRadio added_radio);
	void handle_playlist_update(message::PlaylistUpdate update);
	void handle_stream_request(message::StreamRequest sr);
	void handle_radio_event(message::RadioEvent re);
};
