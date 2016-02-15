#pragma once

#include "delegator.hpp"
#include "library.hpp"
#include "message.hpp"
#include "message_callbacks.hpp"
#include "playlist.hpp"
#include "connection.hpp"
#include "dmp_sender.hpp"
#include "dmp_receiver.hpp"
#include "dmp_client_interface.hpp"
#include "dmp_client_radio_interface.hpp"

#include "message_switch.hpp"

#include <thread>
#include <stdint.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

class PlaylistsModel;
class RadioListModel;
class SearchBarModel;
class SearchResultModel;

namespace boost { namespace asio { class io_service; } }

namespace dmp_library { struct LibraryEntry; }

struct DmpClientUiDelegate;


class DmpClient : public DmpClientInterface, public DmpClientRadioInterface, public std::enable_shared_from_this<DmpClientRadioInterface>, public Delegator<DmpClientUiDelegate>
{
	std::string name;
	std::string host;

public:
	std::shared_ptr<PlaylistsModel> playlists_model;
	std::shared_ptr<RadioListModel> radio_list_model;
	std::shared_ptr<SearchBarModel> search_bar_model;
	std::shared_ptr<SearchResultModel> search_result_model;

private:
	std::shared_ptr<boost::asio::io_service> io_service;
	std::thread library_load_thread;
	std::thread helper_thread;
	boost::asio::deadline_timer library_info_timer;
	message::DmpCallbacks callbacks;
	Connection connection;
	message::Ping last_sent_ping;
	dmp_library::Library library;
	
	std::map<std::string, DmpSender> senders;
	
	DmpReceiver receiver;

	MessageSwitch message_switch;

public:

	DmpClient(std::string host, uint16_t port, bool secure);
	DmpClient(DmpClient&&) = default;
	virtual ~DmpClient();

	message::DmpCallbacks::Callbacks_t initial_callbacks();
	
	virtual std::shared_ptr<PlaylistsModel> get_playlists_model() override final;
	virtual std::shared_ptr<RadioListModel> get_radio_list_model() override final;
	virtual std::shared_ptr<SearchBarModel> get_search_bar_model() override final;
	virtual std::shared_ptr<SearchResultModel> get_search_result_model() override final;
	virtual void add_delegate(std::weak_ptr<DmpClientUiDelegate> delegate) override final;
	virtual std::string get_name() override final;

	void handle_request(message::Type t);
	void listen_requests();

	virtual void clear_model() override final;
	virtual void stop() override final;
	virtual void run() override final;
	virtual void search(std::string query) override final;
	virtual void add_radio(std::string str) override final;
	virtual void remove_radio(std::string str) override final;
	virtual void queue(std::string radio, std::vector<PlaylistEntry> entries) override final;
	virtual void unqueue(std::string radio, std::vector<PlaylistId> ids) override final;
	virtual void set_current_radio(std::string name) override final;
	virtual void tune_in(std::string radio, bool tune_in) override final;
	virtual std::string get_tuned_in_radio() override final;
	virtual void send_bye() override final;
	virtual void send_login(std::string username, std::string password) override final;
	virtual void register_user(std::string username, std::string password) override final;
	virtual void init_library() override final;
	virtual void move_queue(std::string radio, std::vector<uint32_t> playlist_id, bool up) override final;

	virtual void stop_radio() override final;
	virtual void play_radio() override final;
	virtual void pause_radio() override final;
	virtual void next_radio() override final;
	virtual void mute_radio(bool state) override final;
	virtual void change_volume(int volume) override final;
	
	virtual void forward_radio_action(message::RadioAction ra) override final;
	virtual void forward_radio_event(message::SenderEvent se) override final;
	
	virtual void gstreamer_debug(std::string reason) override final;

	void handle_public_key(message::PublicKey pk);
	void handle_login_response(message::LoginResponse lr);
	void handle_register_response(message::RegisterResponse rr);
	void handle_ping(message::Ping ping);
	void handle_pong(message::Pong pong);
	void handle_search_request(message::SearchRequest search_req);
	void handle_search_response(message::SearchResponse search_res);
	void handle_bye_ack(message::ByeAck);
	void handle_add_radio_response(message::AddRadioResponse);
	void handle_remove_radio(message::RemoveRadio);
	void handle_listener_connection_request(message::ListenConnectionRequest req);
	void handle_radios(message::Radios radios);
	void handle_add_radio(message::AddRadio added_radio);
	void handle_stream_request(message::StreamRequest sr);
	void handle_sender_action(message::SenderAction sa);
	void handle_receiver_action(message::ReceiverAction ra);
	void handle_radio_states(message::RadioStates rs);
	void handle_disconnected(message::Disconnected d);
};
