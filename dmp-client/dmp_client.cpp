#include "dmp_client.hpp"

#include "dmp_config.hpp"
#include "fusion_outputter.hpp"
#include "connect.hpp"
#include "timed_debug.hpp"
#include "debug_macros.hpp"
#include "dmp_client_ui_delegate.hpp"

DmpClient::DmpClient(std::string host, uint16_t port)
: name()
, host(host)
, playlists_model(std::make_shared<PlaylistsModel>())
, radio_list_model(std::make_shared<RadioListModel>())
, search_bar_model(std::make_shared<SearchBarModel>())
, search_result_model(std::make_shared<SearchResultModel>())
, io_service(std::make_shared<boost::asio::io_service>())
, callbacks(std::bind(&DmpClient::listen_requests, this), initial_callbacks())
, connection(connect(host, port, io_service))
, last_sent_ping()
, library()
, senders()
, receiver()
, message_switch(make_message_switch(callbacks, connection))
{
	auto library_info = get_library_information();
	if(library_info) {
		for(auto&& entry : library_info.get()){
			DEBUG_COUT << entry.second.get<std::string>("name");
			DEBUG_COUT << entry.second.get<std::string>("path");
			DEBUG_COUT << entry.second.get<std::string>("cache_file");
		}
	} else {
		config_add_library("blaat1", "/path/1", "somesha1hash");
		config_add_library("blaat2", "/path/2", "someotherhash");
	}
}

message::DmpCallbacks::Callbacks_t DmpClient::initial_callbacks()
{
	using std::placeholders::_1;
	return {
		{message::Type::LoginResponse, std::function<bool(message::LoginResponse)>(std::bind(&DmpClient::handle_login_response, this, _1))},
		{message::Type::RegisterResponse, std::function<bool(message::RegisterResponse)>(std::bind(&DmpClient::handle_register_response, this, _1))},
		{message::Type::Ping, std::function<bool(message::Ping)>(std::bind(&DmpClient::handle_ping, this, _1))},
		{message::Type::Pong, std::function<bool(message::Pong)>(std::bind(&DmpClient::handle_pong, this, _1))},
		{message::Type::SearchRequest, std::function<bool(message::SearchRequest)>(std::bind(&DmpClient::handle_search_request, this, _1))},
		{message::Type::SearchResponse, std::function<bool(message::SearchResponse)>(std::bind(&DmpClient::handle_search_response, this, _1))},
		{message::Type::ByeAck, std::function<bool(message::ByeAck)>(std::bind(&DmpClient::handle_bye_ack, this, _1))},
		{message::Type::AddRadioResponse, std::function<bool(message::AddRadioResponse)>(std::bind(&DmpClient::handle_add_radio_response, this, _1))},
		{message::Type::ListenConnectionRequest, std::function<bool(message::ListenConnectionRequest)>(std::bind(&DmpClient::handle_listener_connection_request, this, _1))},
		{message::Type::Radios, std::function<bool(message::Radios)>(std::bind(&DmpClient::handle_radios, this, _1))},
		{message::Type::AddRadio, std::function<bool(message::AddRadio)>(std::bind(&DmpClient::handle_add_radio, this, _1))},
		{message::Type::RemoveRadio, std::function<bool(message::RemoveRadio)>(std::bind(&DmpClient::handle_remove_radio, this, _1))},
		{message::Type::PlaylistUpdate, std::function<bool(message::PlaylistUpdate)>(std::bind(&PlaylistsModel::handle_update, playlists_model.get(), _1))},
		{message::Type::StreamRequest, std::function<bool(message::StreamRequest)>(std::bind(&DmpClient::handle_stream_request, this, _1))},
		{message::Type::SenderAction, std::function<bool(message::SenderAction)>(std::bind(&DmpClient::handle_sender_action, this, _1))},
		{message::Type::ReceiverAction, std::function<bool(message::ReceiverAction)>(std::bind(&DmpClient::handle_receiver_action, this, _1))},
		{message::Type::RadioStates, std::function<bool(message::RadioStates)>(std::bind(&DmpClient::handle_radio_states, this, _1))}
	};
}

std::shared_ptr<PlaylistsModel> DmpClient::get_playlists_model()
{
	return playlists_model;
}

std::shared_ptr<RadioListModel> DmpClient::get_radio_list_model()
{
	return radio_list_model;
}

std::shared_ptr<SearchBarModel> DmpClient::get_search_bar_model()
{
	return search_bar_model;
}

std::shared_ptr<SearchResultModel> DmpClient::get_search_result_model()
{
	return search_result_model;
}

void DmpClient::add_delegate(std::weak_ptr<DmpClientUiDelegate> delegate)
{
	Delegator::add_delegate<DmpClientUiDelegate>(delegate);
}

void DmpClient::run()
{
	listen_requests();
	io_service->run();
}

void DmpClient::destroy()
{
	io_service->stop();
}

void DmpClient::stop()
{
	message::Bye b;
	connection.send_encrypted(b);
}

void DmpClient::index(std::string path)
{
	library = dmp_library::create_library(path);
}

void DmpClient::add_radio(std::string radio_name)
{
	connection.send_encrypted(message::AddRadio(radio_name));
}

void DmpClient::remove_radio(std::string str)
{
	connection.send_encrypted(message::RemoveRadio(str));
}

void DmpClient::queue(std::string radio, std::string owner, dmp_library::LibraryEntry entry)
{
	connection.send_encrypted(message::Queue(radio, name, owner, entry));
}

void DmpClient::set_current_radio(std::string name)
{
	message::PlaylistUpdate::Action action(message::PlaylistUpdate::Action::Type::Reset, 0, 0);
	message::PlaylistUpdate update(action, name, {});
	
	playlists_model->set_current_radio(name);
}

void DmpClient::tune_in(std::string radio, bool tune_in)
{
	using Action = message::TuneIn::Action;
	Action action = Action::NoAction;
	if(tune_in) {
		action = Action::TuneIn;
		radio_list_model->set_tuned_in_radio(radio);
	} else {
		action = Action::TuneOff;
		radio_list_model->set_tuned_in_radio("");
	}
	message::TuneIn ti(radio, action);
	connection.send_encrypted(ti);
}

std::string DmpClient::get_tuned_in_radio()
{
	return receiver.radio_target();
}

void DmpClient::send_bye()
{
	connection.send_encrypted(message::Bye());
}

void DmpClient::send_login(std::string username, std::string password)
{
	connection.send_encrypted(message::LoginRequest(username, password));
	name = username;
}

void DmpClient::register_user(std::string username, std::string password)
{
	connection.send_encrypted(message::RegisterRequest(username, password));
}

void DmpClient::handle_request(message::Type t)
{
	message_switch.handle_message(t);
}

void DmpClient::listen_requests()
{
	std::function<void(message::Type)> cb = std::bind(&DmpClient::handle_request, this, std::placeholders::_1);
	connection.async_receive_encrypted_type(cb);
}

void DmpClient::search(std::string query)
{
	search_bar_model->set_query(query);
	try {
		dmp_library::parse_query(query);
	} catch (dmp_library::ParseError &err) {
		search_bar_model->set_data(err.expected, err.pivot);
		return;
	}

	search_result_model->set_current_query(query);
	
	message::SearchRequest q(query);
	connection.send_encrypted(q);
}

void DmpClient::stop_radio()
{
	connection.send_encrypted(message::RadioAction(playlists_model->get_current_radio(), message::PlaybackAction::Stop));
}

void DmpClient::play_radio()
{
	connection.send_encrypted(message::RadioAction(playlists_model->get_current_radio(), message::PlaybackAction::Play));
}

void DmpClient::pause_radio()
{
	connection.send_encrypted(message::RadioAction(playlists_model->get_current_radio(), message::PlaybackAction::Pause));
}

void DmpClient::next_radio()
{
	connection.send_encrypted(message::RadioAction(playlists_model->get_current_radio(), message::PlaybackAction::Next));
}

void DmpClient::forward_radio_action(message::RadioAction ra)
{
	connection.send_encrypted(ra);
}

void DmpClient::forward_radio_event(message::SenderEvent se)
{
	connection.send_encrypted(se);
}

bool DmpClient::handle_login_response(message::LoginResponse lr)
{
	if(lr.succes) {
		call_on_delegates<DmpClientUiDelegate>(&DmpClientUiDelegate::login_succeeded);
	} else {
		call_on_delegates<DmpClientUiDelegate>(&DmpClientUiDelegate::login_failed, lr.reason);
		name = "";
	}
	return true;
}

bool DmpClient::handle_register_response(message::RegisterResponse rr)
{
	if(rr.succes) {
		call_on_delegates<DmpClientUiDelegate>(&DmpClientUiDelegate::register_succeeded);
	} else {
		call_on_delegates<DmpClientUiDelegate>(&DmpClientUiDelegate::register_failed, rr.reason);
	}
	return true;
}

void DmpClient::mute_radio(bool state)
{
	receiver.mute(state);
}

bool DmpClient::handle_ping(message::Ping ping)
{
	message::Pong pong(ping.payload);
	connection.send_encrypted(pong);
	return true;
}

bool DmpClient::handle_pong(message::Pong pong)
{
	assert(last_sent_ping.payload != "");
	if (last_sent_ping.payload != pong.payload) {
		stop();
	}
	return true;
}

bool DmpClient::handle_search_request(message::SearchRequest search_req)
{
	dmp_library::LibrarySearcher searcher(library);
	message::SearchResponse response(name, search_req.query, searcher.search(search_req.query));
	if(!response.results.empty()) {
		connection.send_encrypted(response);
	}
	return true;
}

bool DmpClient::handle_search_response(message::SearchResponse search_res)
{
	search_result_model->add_search_response(search_res);
	return true;
}

bool DmpClient::handle_bye_ack(message::ByeAck)
{
	io_service->stop();
	return false;
}

bool DmpClient::handle_add_radio_response(message::AddRadioResponse response)
{
	if(response.succes) {
		call_on_delegates<DmpClientUiDelegate>(&DmpClientUiDelegate::add_radio_succes, response);
	} else {
		call_on_delegates<DmpClientUiDelegate>(&DmpClientUiDelegate::add_radio_failed, response);
	}
	return true;
}

bool DmpClient::handle_remove_radio(message::RemoveRadio rr)
{
	radio_list_model->remove_radio(rr.name);
	playlists_model->remove_radio(rr.name);
	return true;
}

bool DmpClient::handle_listener_connection_request(message::ListenConnectionRequest req)
{
	receiver.stop_loop();
	if(receiver_thread.joinable()) {
		receiver_thread.join();
	}
	receiver_thread = std::thread(std::bind(&DmpReceiver::run_loop, std::ref(receiver)));
	receiver.setup(req.radio_name, host, req.port);
	receiver.pause();
	return true;
}

bool DmpClient::handle_radios(message::Radios radios)
{
	for(auto&& radio : radios.radios)
	{
		radio_list_model->add_radio(radio.first);
		playlists_model->update(radio.first, radio.second);
	}
	return true;
}

DmpClient::~DmpClient() {
	receiver.stop_loop();
	if(receiver_thread.joinable()) {
		receiver_thread.join();
	}
}

bool DmpClient::handle_add_radio(message::AddRadio added_radio)
{
	radio_list_model->add_radio(added_radio.name);
	playlists_model->create_radio(added_radio.name);
	return true;
}

bool DmpClient::handle_stream_request(message::StreamRequest sr)
{
	senders.erase(sr.radio_name);
	auto result = senders.emplace(sr.radio_name, DmpSender(shared_from_this(), sr.radio_name));
	
	if(!result.second) {
		DEBUG_COUT << "emplace did not overwrite existing map entry" << std::endl;
	}
	
	auto sender_runner = [this, sr]{
		try {
			senders.at(sr.radio_name).run_loop();
		} catch(std::exception &e){
			DEBUG_COUT << "Sender crashed with message: " << e.what() << std::endl;
		}
	};
	std::thread t(sender_runner);
	
	t.detach();
	
	senders.at(sr.radio_name).setup(host, sr.port, library.get_filename(sr.entry));
	return true;
}

bool DmpClient::handle_sender_action(message::SenderAction sa)
{
	auto sender_it = senders.find(sa.radio_name);
	if(sender_it != senders.end()) {
		auto& sender = sender_it->second;
		switch(sa.action)
		{
			case message::PlaybackAction::Pause:
			{
				sender.pause();
				break;
			}
			case message::PlaybackAction::Play:
			{
				sender.play();
				break;
			}
			case message::PlaybackAction::Stop:
			{
				sender.stop();
				break;
			}
			case message::PlaybackAction::Reset:
			{
				sender.reset();
				break;
			}
			//explicit falltrough.
			case message::PlaybackAction::NoAction:
			case message::PlaybackAction::Next:
			default:
			{
				throw std::runtime_error("SenderAction with incompatible command found in dmp_client, command was: " + std::to_string(static_cast<message::Type_t>(sa.action)));
			}
		}
	}
	return true;
}

bool DmpClient::handle_receiver_action(message::ReceiverAction ra)
{
	switch(ra.action)
	{
		case message::PlaybackAction::Pause:
		{
			receiver.pause();
			call_on_delegates<DmpClientUiDelegate>(&DmpClientUiDelegate::set_play_paused_state, false);
			break;
		}
		case message::PlaybackAction::Play:
		{
			receiver.play();
			call_on_delegates<DmpClientUiDelegate>(&DmpClientUiDelegate::set_play_paused_state, true);
			break;
		}
		case message::PlaybackAction::Stop:
		{
			receiver.stop();
			call_on_delegates<DmpClientUiDelegate>(&DmpClientUiDelegate::set_play_paused_state, false);
			break;
		}
		//explicit falltrough
		case message::PlaybackAction::Next:
		case message::PlaybackAction::NoAction:
		case message::PlaybackAction::Reset:
		default:
		{
			throw std::runtime_error("ReceiverAction with incompatible command found in dmp_client, command was: " + std::to_string(static_cast<message::Type_t>(ra.action)));
		}
	}
	return true;
}

bool DmpClient::handle_radio_states(message::RadioStates rs)
{
	switch(rs.action) {
		case message::RadioStates::Action::Set:
		{
			radio_list_model->set_radio_states(rs.states);
			break;
		}
		case message::RadioStates::Action::NoAction:
		default:
		{
			throw std::runtime_error("Received a RadioStates message with unhandled action: " + std::to_string(static_cast<message::Type_t>(rs.action)));
		}
	}
	return true;
}
