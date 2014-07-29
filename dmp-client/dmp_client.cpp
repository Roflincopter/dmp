#include "dmp_client.hpp"

#include "message_outputter.hpp"
#include "connect.hpp"
#include "timed_debug.hpp"
#include "debug_macros.hpp"

DmpClient::DmpClient(std::string name, std::string host, uint16_t port)
: name(name)
, host(host)
, callbacks(std::bind(&DmpClient::listen_requests, this), initial_callbacks())
, connection(dmp::connect(host, port))
, last_sent_ping()
, library()
, delegates()
, senders()
, receiver()
, message_switch(make_message_switch(callbacks, connection))
, playlists_model(std::make_shared<PlaylistsModel>())
, radio_list_model(std::make_shared<RadioListModel>())
, search_bar_model(std::make_shared<SearchBarModel>())
, search_result_model(std::make_shared<SearchResultModel>())
{
	timed_debug::add_call([this, name]{receiver.make_debug_graph(name);});

	timed_debug::add_call([this]{
		for(auto&& sender : senders) {
			sender.second.make_debug_graph(sender.first);
		}
	});
}

message::DmpCallbacks::Callbacks_t DmpClient::initial_callbacks()
{
	using std::placeholders::_1;
	return {
		{message::Type::Ping, std::function<void(message::Ping)>(std::bind(&DmpClient::handle_ping, this, _1))},
		{message::Type::Pong, std::function<void(message::Ping)>(std::bind(&DmpClient::handle_pong, this, _1))},
		{message::Type::NameRequest, std::function<void(message::NameRequest)>(std::bind(&DmpClient::handle_name_request, this, _1))},
		{message::Type::SearchRequest, std::function<void(message::SearchRequest)>(std::bind(&DmpClient::handle_search_request, this, _1))},
		{message::Type::SearchResponse, std::function<void(message::SearchResponse)>(std::bind(&DmpClient::handle_search_response, this, _1))},
		{message::Type::ByeAck, std::function<void(message::ByeAck)>(std::bind(&DmpClient::handle_bye_ack, this, _1))},
		{message::Type::AddRadioResponse, std::function<void(message::AddRadioResponse)>(std::bind(&DmpClient::handle_add_radio_response, this, _1))},
		{message::Type::ListenConnectionRequest, std::function<void(message::ListenConnectionRequest)>(std::bind(&DmpClient::handle_listener_connection_request, this, _1))},
		{message::Type::Radios, std::function<void(message::Radios)>(std::bind(&DmpClient::handle_radios, this, _1))},
		{message::Type::AddRadio, std::function<void(message::AddRadio)>(std::bind(&DmpClient::handle_add_radio, this, _1))},
		{message::Type::PlaylistUpdate, std::function<void(message::PlaylistUpdate)>(std::bind(&DmpClient::handle_playlist_update, this, _1))},
		{message::Type::StreamRequest, std::function<void(message::StreamRequest)>(std::bind(&DmpClient::handle_stream_request, this, _1))},
		{message::Type::SenderAction, std::function<void(message::SenderAction)>(std::bind(&DmpClient::handle_sender_action, this, _1))},
		{message::Type::ReceiverAction, std::function<void(message::ReceiverAction)>(std::bind(&DmpClient::handle_receiver_action, this, _1))}
	};
}

void DmpClient::add_delegate(std::weak_ptr<DmpClientUiDelegate> delegate)
{
	delegates.push_back(delegate);
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

void DmpClient::run()
{
	listen_requests();
	connection.io_service->run();
}

void DmpClient::destroy()
{
	connection.io_service->stop();
}

void DmpClient::stop()
{
	message::Bye b;
	connection.send(b);
}

void DmpClient::index(std::string path)
{
	library = dmp_library::create_library(path);
}

void DmpClient::add_radio(std::string radio_name)
{
	connection.send(message::AddRadio(radio_name));
}

void DmpClient::queue(std::string radio, std::string owner, dmp_library::LibraryEntry entry)
{
	connection.send(message::Queue(radio, name, owner, entry));
}

void DmpClient::set_current_radio(std::string name)
{
	message::PlaylistUpdate::Action a(message::PlaylistUpdate::Action::Type::Reset, 0, 0);
	message::PlaylistUpdate u(a, name, {});
	
	call_on_delegates(delegates, &DmpClientUiDelegate::playlist_update_start, u);
	playlists_model->set_current_radio(name);
	call_on_delegates(delegates, &DmpClientUiDelegate::playlist_update_end, u);
}

void DmpClient::tune_in(std::string radio)
{
	message::TuneIn ti(radio);
	connection.send(ti);
}

void DmpClient::handle_request(message::Type t)
{
	message_switch.handle_message(t);
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
		call_on_delegates(delegates, &DmpClientUiDelegate::query_parse_error);
		return;
	}

	call_on_delegates(delegates, &DmpClientUiDelegate::new_search_begin);
	search_result_model->clear();
	search_result_model->set_current_query(query);
	call_on_delegates(delegates, &DmpClientUiDelegate::new_search_end);
	
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

void DmpClient::mute_radio(bool state)
{
	receiver.mute(state);
}

void DmpClient::handle_ping(message::Ping ping)
{
	message::Pong pong(ping);
	connection.send(pong);
}

void DmpClient::handle_pong(message::Pong pong)
{
	assert(last_sent_ping.payload != "");
	if (last_sent_ping.payload != pong.payload) {
		stop();
	}
}

void DmpClient::handle_name_request(message::NameRequest name_req)
{
	message::NameResponse name_res(name);
	connection.send(name_res);
}

void DmpClient::handle_search_request(message::SearchRequest search_req)
{
	dmp_library::LibrarySearcher searcher(library);
	message::SearchResponse response(search_req.query, searcher.search(search_req.query), name);
	connection.send(response);
}

void DmpClient::handle_search_response(message::SearchResponse search_res)
{
	call_on_delegates(delegates, &DmpClientUiDelegate::search_results_start, search_res);
	search_result_model->add_search_response(search_res);
	call_on_delegates(delegates, &DmpClientUiDelegate::search_results_end);
}

void DmpClient::handle_bye_ack(message::ByeAck)
{
	connection.io_service->stop();
	call_on_delegates(delegates, &DmpClientUiDelegate::client_stopped);
}

void DmpClient::handle_add_radio_response(message::AddRadioResponse response)
{
	if(response.succes) {
		call_on_delegates(delegates, &DmpClientUiDelegate::add_radio_succes, response);
	} else {
		call_on_delegates(delegates, &DmpClientUiDelegate::add_radio_failed, response);
	}
}

void DmpClient::handle_listener_connection_request(message::ListenConnectionRequest req)
{
	receiver.stop_loop();
	if(receiver_thread.joinable()) {
		receiver_thread.join();
	}
	receiver_thread = std::thread(std::bind(&DmpReceiver::run_loop, std::ref(receiver)));
	receiver.setup(req.radio_name, host, req.port);
	receiver.pause();
}

void DmpClient::handle_radios(message::Radios radios)
{
	call_on_delegates(delegates, &DmpClientUiDelegate::set_radios_start);
	for(auto&& radio : radios.radios)
	{
		radio_list_model->add_radio(radio.first);
		playlists_model->update(radio.first, radio.second);
	}
	call_on_delegates(delegates, &DmpClientUiDelegate::set_radios_end);
}

DmpClient::~DmpClient() {
	receiver.stop_loop();
	if(receiver_thread.joinable()) {
		receiver_thread.join();
	}
}

void DmpClient::handle_add_radio(message::AddRadio added_radio)
{
	call_on_delegates(delegates, &DmpClientUiDelegate::add_radio_start);
	radio_list_model->add_radio(added_radio.name);
	playlists_model->create_radio(added_radio.name);
	call_on_delegates(delegates, &DmpClientUiDelegate::add_radio_end);
}

void DmpClient::handle_playlist_update(message::PlaylistUpdate update)
{
	call_on_delegates(delegates, &DmpClientUiDelegate::playlist_update_start, update);
	playlists_model->handle_update(update);
	call_on_delegates(delegates, &DmpClientUiDelegate::playlist_update_end, update);
}
void DmpClient::handle_stream_request(message::StreamRequest sr)
{
	//remove this when todo is fixed
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
		//TODO: fix this more generic by a templated struct with a std::thread and a T struct as mapped type.
		//senders.erase(sr.radio_name);
	};
	std::thread t(sender_runner);
	
	t.detach();
	
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
			call_on_delegates(delegates, &DmpClientUiDelegate::set_play_paused_state, false);
			break;
		}
		case message::PlaybackAction::Play:
		{
			receiver.play();
			call_on_delegates(delegates, &DmpClientUiDelegate::set_play_paused_state, true);
			break;
		}
		case message::PlaybackAction::Stop:
		{
			receiver.stop();
			call_on_delegates(delegates, &DmpClientUiDelegate::set_play_paused_state, false);
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
