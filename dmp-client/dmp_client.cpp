#include "dmp_client.hpp"

#include "key_management.hpp"

#include "library-entry.hpp"
#include "playlists_model.hpp"
#include "radio_list_model.hpp"
#include "radio_state.hpp"
#include "search_bar_model.hpp"
#include "search_result_model.hpp"
#include "library-search.hpp"
#include "query-parser.hpp"
#include "dmp_config.hpp"
#include "connect.hpp"
#include "debug_macros.hpp"
#include "dmp_client_ui_delegate.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/serialization/serialization.hpp>

#include <assert.h>
#include <cstdint>
#include <exception>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <utility>

DmpClient::DmpClient(std::string host, uint16_t port, bool secure)
: name()
, host(host)
, playlists_model(std::make_shared<PlaylistsModel>())
, radio_list_model(std::make_shared<RadioListModel>())
, search_bar_model(std::make_shared<SearchBarModel>())
, search_result_model(std::make_shared<SearchResultModel>())
, io_service(std::make_shared<boost::asio::io_service>())
, library_load_thread()
, helper_thread()
, library_info_timer(*io_service)
, callbacks(std::bind(&DmpClient::listen_requests, this), initial_callbacks(), io_service)
, connection(connect(host, port, io_service))
, last_sent_ping()
, library()
, senders()
, receiver(config::get_gst_folder_name().string())
, message_switch(make_message_switch())
{
	if(secure) {
		KeyPair kp = load_key_pair();
		connection.set_our_keys(kp.private_key, kp.public_key);
		connection.send(message::PublicKey(kp.public_key));
	}

	auto volume = config::get_volume();
	change_volume(volume);
	init_library();
}

message::DmpCallbacks::Callbacks_t DmpClient::initial_callbacks()
{
	using std::placeholders::_1;
	return {
		{message::Type::LoginResponse, std::function<void(message::LoginResponse)>(std::bind(&DmpClient::handle_login_response, this, _1))},
		{message::Type::RegisterResponse, std::function<void(message::RegisterResponse)>(std::bind(&DmpClient::handle_register_response, this, _1))},
		{message::Type::Ping, std::function<void(message::Ping)>(std::bind(&DmpClient::handle_ping, this, _1))},
		{message::Type::Pong, std::function<void(message::Pong)>(std::bind(&DmpClient::handle_pong, this, _1))},
		{message::Type::SearchRequest, std::function<void(message::SearchRequest)>(std::bind(&DmpClient::handle_search_request, this, _1))},
		{message::Type::SearchResponse, std::function<void(message::SearchResponse)>(std::bind(&DmpClient::handle_search_response, this, _1))},
		{message::Type::ByeAck, std::function<void(message::ByeAck)>(std::bind(&DmpClient::handle_bye_ack, this, _1))},
		{message::Type::AddRadioResponse, std::function<void(message::AddRadioResponse)>(std::bind(&DmpClient::handle_add_radio_response, this, _1))},
		{message::Type::ListenConnectionRequest, std::function<void(message::ListenConnectionRequest)>(std::bind(&DmpClient::handle_listener_connection_request, this, _1))},
		{message::Type::Radios, std::function<void(message::Radios)>(std::bind(&DmpClient::handle_radios, this, _1))},
		{message::Type::AddRadio, std::function<void(message::AddRadio)>(std::bind(&DmpClient::handle_add_radio, this, _1))},
		{message::Type::RemoveRadio, std::function<void(message::RemoveRadio)>(std::bind(&DmpClient::handle_remove_radio, this, _1))},
		{message::Type::PlaylistUpdate, std::function<void(message::PlaylistUpdate)>(std::bind(&PlaylistsModel::handle_update, playlists_model.get(), _1))},
		{message::Type::StreamRequest, std::function<void(message::StreamRequest)>(std::bind(&DmpClient::handle_stream_request, this, _1))},
		{message::Type::SenderAction, std::function<void(message::SenderAction)>(std::bind(&DmpClient::handle_sender_action, this, _1))},
		{message::Type::ReceiverAction, std::function<void(message::ReceiverAction)>(std::bind(&DmpClient::handle_receiver_action, this, _1))},
		{message::Type::RadioStates, std::function<void(message::RadioStates)>(std::bind(&DmpClient::handle_radio_states, this, _1))},
		{message::Type::PublicKey, std::function<void(message::PublicKey)>(std::bind(&DmpClient::handle_public_key, this, _1))},
		{message::Type::Disconnected, std::function<void(message::Disconnected)>(std::bind(&DmpClient::handle_disconnected, this, _1))}
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
	call_on_delegates(&DmpClientUiDelegate::volume_changed, receiver.get_volume());
}

std::string DmpClient::get_name() {
	return name;
}

void DmpClient::run()
{
	listen_requests();
	helper_thread = std::thread([this](){
		io_service->run();
	});
	io_service->run();
}

void DmpClient::clear_model()
{
	playlists_model->clear();
	radio_list_model->clear();
	search_bar_model->clear();
	search_result_model->clear();
}

void DmpClient::stop()
{
	connection.stop_encryption();
	message::Bye b;
	connection.send(b);
}

void DmpClient::add_radio(std::string radio_name)
{
	connection.send(message::AddRadio(radio_name));
}

void DmpClient::remove_radio(std::string str)
{
	connection.send(message::RemoveRadio(str));
}

void DmpClient::queue(std::string radio, std::vector<PlaylistEntry> entries)
{
	message::PlaylistUpdate::Action append(message::PlaylistUpdate::Action::Type::Append, {});
	connection.send(message::PlaylistUpdate(append, radio, entries));
}

void DmpClient::unqueue(std::string radio, std::vector<PlaylistId> ids)
{
	message::PlaylistUpdate::Action remove(message::PlaylistUpdate::Action::Type::Remove, ids);
	
	connection.send(message::PlaylistUpdate(remove, radio, {}));
}

void DmpClient::set_current_radio(std::string name)
{
	message::PlaylistUpdate::Action action(message::PlaylistUpdate::Action::Type::Reset, {});
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
	connection.send(ti);
}

std::string DmpClient::get_tuned_in_radio()
{
	return receiver.radio_target();
}

void DmpClient::send_bye()
{
	connection.send(message::Bye());
}

void DmpClient::send_login(std::string username, std::string password)
{
	connection.send(message::LoginRequest(username, password));
	name = username;
}

void DmpClient::register_user(std::string username, std::string password)
{
	connection.send(message::RegisterRequest(username, password));
}

std::function<void(boost::system::error_code const&)> timer_callback;

void DmpClient::init_library()
{
	//TODO: notify other clients of the library change.
	library.load_info->should_stop = true;
	if(library_load_thread.joinable()) {
		library_load_thread.join();
	}
	library.clear();
	
	try {
		library_load_thread = std::thread([this](){
			io_service->post([this](){
			
				call_on_delegates(&DmpClientUiDelegate::library_load_start);
				
				auto perpetuation = [this](){
					library_info_timer.expires_from_now(boost::posix_time::millisec(100));
					library_info_timer.async_wait(timer_callback);
				};
				
				timer_callback = [this, perpetuation](boost::system::error_code const& ec) {
				
					if(ec) {
						if(ec.value() == boost::system::errc::operation_canceled) {
							return;
						}
						throw std::runtime_error("something went wrong library load info timer.");
					}
					
					call_on_delegates(&DmpClientUiDelegate::library_load_info, library.load_info);
					perpetuation();
					
				};
				perpetuation();
				
			});
			
			auto&& library_info = config::get_library_information();
		
			library.load_library(library_info);
			
			io_service->post([this](){
				library_info_timer.cancel();
				call_on_delegates(&DmpClientUiDelegate::library_load_end);
			});
		});
	} catch(dmp_library::Interrupted const&) {
		return;
	}
	
}

void DmpClient::move_queue(std::string radio, std::vector<uint32_t> playlist_id, bool up)
{
	using type = message::PlaylistUpdate::Action::Type;
	connection.send(message::PlaylistUpdate(
		message::PlaylistUpdate::Action(
			up ? type::MoveUp : type::MoveDown, playlist_id),
		radio,
		{}
	));
}

void DmpClient::handle_request(message::Type t)
{
	message_switch.handle_message(connection, t, callbacks);
}

void DmpClient::listen_requests()
{
	std::function<void(message::Type)> cb = std::bind(&DmpClient::handle_request, this, std::placeholders::_1);
	connection.async_receive_type(cb);
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
	connection.send(q);
}

void DmpClient::stop_radio()
{
	connection.send(message::RadioAction(playlists_model->get_current_radio(), message::PlaybackAction::Stop));
}

void DmpClient::play_radio()
{
	connection.send(message::RadioAction(playlists_model->get_current_radio(), message::PlaybackAction::Play));
}

void DmpClient::pause_radio()
{
	connection.send(message::RadioAction(playlists_model->get_current_radio(), message::PlaybackAction::Pause));
}

void DmpClient::next_radio()
{
	connection.send(message::RadioAction(playlists_model->get_current_radio(), message::PlaybackAction::Next));
}

void DmpClient::forward_radio_action(message::RadioAction ra)
{
	connection.send(ra);
}

void DmpClient::forward_radio_event(message::SenderEvent se)
{
	connection.send(se);
}

void DmpClient::gstreamer_debug(std::string reason)
{
	std::cout << "created debug graph: " << receiver.make_debug_graph(reason) << std::endl;
	for(auto&& sender : senders) {
		std::cout << "created debug graph: " << sender.second.make_debug_graph(sender.first + "_" + reason) << std::endl;
	}
}

void DmpClient::handle_public_key(message::PublicKey pk)
{
	connection.set_their_key(pk.key);
}

void DmpClient::handle_login_response(message::LoginResponse lr)
{
	if(lr.succes) {
		call_on_delegates(&DmpClientUiDelegate::login_succeeded);
	} else {
		call_on_delegates(&DmpClientUiDelegate::login_failed, lr.reason);
		name = "";
	}
}

void DmpClient::handle_register_response(message::RegisterResponse rr)
{
	if(rr.succes) {
		call_on_delegates(&DmpClientUiDelegate::register_succeeded);
	} else {
		call_on_delegates(&DmpClientUiDelegate::register_failed, rr.reason);
	}
}

void DmpClient::mute_radio(bool state)
{
	receiver.mute(state);
}

void DmpClient::change_volume(int volume)
{
	receiver.set_volume(volume);
	config::set_volume(volume);
	call_on_delegates(&DmpClientUiDelegate::volume_changed, volume);
}

void DmpClient::handle_ping(message::Ping ping)
{
	message::Pong pong(ping.payload);
	connection.send(pong);
}

void DmpClient::handle_pong(message::Pong pong)
{
	assert(last_sent_ping.payload != "");
	if (last_sent_ping.payload != pong.payload) {
		stop();
	}
}

void DmpClient::handle_search_request(message::SearchRequest search_req)
{
	dmp_library::LibrarySearcher searcher(library);
	message::SearchResponse response(name, search_req.query, searcher.search(search_req.query));
	connection.send(response);
}

void DmpClient::handle_search_response(message::SearchResponse search_res)
{
	search_result_model->add_search_response(search_res);
}

void DmpClient::handle_bye_ack(message::ByeAck)
{
	if(library_load_thread.joinable()) {
		library_load_thread.join();
	}
	for(auto&& sender : senders) {
		sender.second.stop();
	}
	io_service->stop();
}

void DmpClient::handle_add_radio_response(message::AddRadioResponse response)
{
	if(response.succes) {
		call_on_delegates(&DmpClientUiDelegate::add_radio_succes, response);
	} else {
		call_on_delegates(&DmpClientUiDelegate::add_radio_failed, response);
	}
}

void DmpClient::handle_remove_radio(message::RemoveRadio rr)
{
	radio_list_model->remove_radio(rr.name);
	playlists_model->remove_radio(rr.name);
}

void DmpClient::handle_listener_connection_request(message::ListenConnectionRequest req)
{
	receiver.setup(req.radio_name, host, req.port);
	receiver.pause();
}

void DmpClient::handle_radios(message::Radios radios)
{
	for(auto&& radio : radios.radios)
	{
		radio_list_model->add_radio(radio.first);
		playlists_model->update(radio.first, radio.second);
	}
}

DmpClient::~DmpClient() {
	while(!io_service->stopped()) {
		io_service->poll_one();
	}
	if(helper_thread.joinable()) {
		helper_thread.join();
	}
}

void DmpClient::handle_add_radio(message::AddRadio added_radio)
{
	radio_list_model->add_radio(added_radio.name);
	playlists_model->create_radio(added_radio.name);
}

void DmpClient::handle_stream_request(message::StreamRequest sr)
{
	senders.erase(sr.radio_name);
	senders.emplace(
		sr.radio_name,
		DmpSender(
			shared_from_this(),
			sr.radio_name,
			config::get_gst_folder_name().string()
		)
	);
		
	try {
		std::string file_name =  library.get_filename(sr.entry);
	} catch(dmp_library::EntryNotFound const&) {
		std::cout << "Caught exception" << std::endl;
		senders.at(sr.radio_name).eos_reached();
	}
	
	senders.at(sr.radio_name).setup(host, sr.port, library.get_filename(sr.entry));
}

void DmpClient::handle_sender_action(message::SenderAction sa)
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
}

void DmpClient::handle_receiver_action(message::ReceiverAction ra)
{
	switch(ra.action)
	{
		case message::PlaybackAction::Pause:
		{
			receiver.pause();
			call_on_delegates(&DmpClientUiDelegate::set_play_paused_state, false);
			break;
		}
		case message::PlaybackAction::Play:
		{
			receiver.play();
			call_on_delegates(&DmpClientUiDelegate::set_play_paused_state, true);
			break;
		}
		case message::PlaybackAction::Stop:
		{
			receiver.stop();
			call_on_delegates(&DmpClientUiDelegate::set_play_paused_state, false);
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
}

void DmpClient::handle_radio_states(message::RadioStates rs)
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
}

void DmpClient::handle_disconnected(message::Disconnected d)
{
	search_result_model->remove_entries_from(d.name);
}
