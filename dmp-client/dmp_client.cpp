#include "dmp_client.hpp"

#include "message_outputter.hpp"
#include "connect.hpp"

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
, debug_timer(*connection.io_service)
, playlists_model(std::make_shared<PlaylistsModel>())
, radio_list_model(std::make_shared<RadioListModel>())
, search_bar_model(std::make_shared<SearchBarModel>())
, search_result_model(std::make_shared<SearchResultModel>())
{
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
		{message::Type::RadioEvent, std::function<void(message::RadioEvent)>(std::bind(&DmpClient::handle_radio_event, this, _1))}
	};
}

void DmpClient::timed_debug()
{
	debug_timer.expires_from_now(boost::posix_time::seconds(5));

	auto cb = [this](boost::system::error_code ec){
		if(ec)
		{
			throw std::runtime_error("something went wrong in the client debug timer.");
		}
		
		
		
		timed_debug();
	};
	debug_timer.async_wait(cb);
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
	timed_debug();
	listen_requests();
	connection.io_service->run();
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
	try {
		dmp_library::parse_query(query);
	} catch (dmp_library::ParseError err) {
		call_on_delegates(delegates, &DmpClientUiDelegate::query_parse_error, err);
		return;
	}

	call_on_delegates(delegates, &DmpClientUiDelegate::new_search, query);
	message::SearchRequest q(query);
	connection.send(q);
}

void DmpClient::stop_radio(std::string radio_name)
{
	connection.send(message::RadioEvent(radio_name, message::RadioEvent::Action::Stop, {}));
}

void DmpClient::play_radio(std::string radio_name)
{
	connection.send(message::RadioEvent(radio_name, message::RadioEvent::Action::Play, {}));
}

void DmpClient::pause_radio(std::string radio_name)
{
	connection.send(message::RadioEvent(radio_name, message::RadioEvent::Action::Pause, {}));
}

void DmpClient::next_radio(std::string radio_name)
{
	connection.send(message::RadioEvent(radio_name, message::RadioEvent::Action::Next, {}));
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
	call_on_delegates(delegates, &DmpClientUiDelegate::search_results, search_res);
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
	receiver.setup(host, req.port);
	receiver.play();
}

void DmpClient::handle_radios(message::Radios radios)
{
	call_on_delegates(delegates, &DmpClientUiDelegate::radios_update, radios);
}

DmpClient::~DmpClient() {
	receiver.stop_loop();
	if(receiver_thread.joinable()) {
		receiver_thread.join();
	}
}

void DmpClient::handle_add_radio(message::AddRadio added_radio)
{
	call_on_delegates(delegates, &DmpClientUiDelegate::radio_added, added_radio);
}

void DmpClient::handle_playlist_update(message::PlaylistUpdate update)
{
	call_on_delegates(delegates, &DmpClientUiDelegate::playlist_updated, update);
}

void DmpClient::handle_stream_request(message::StreamRequest sr)
{	
	senders.emplace(sr.radio_name, DmpSender());
	auto sender_runner = [this, sr]{
		try {
			senders.at(sr.radio_name).run_loop();
		} catch(std::runtime_error e){
			std::cout << "Sender crashed with message: " << e.what() << std::endl;
		}
	};
	std::thread t(sender_runner);
	t.detach();
	
	senders.at(sr.radio_name).setup(host, sr.port, library.get_filename(sr.entry));
}

void DmpClient::handle_radio_event(message::RadioEvent re)
{
	DmpSender& sender = senders.at(re.radio_name);
	switch(re.action)
	{
		case message::RadioEvent::Action::Pause:
		{
			sender.pause();
			break;
		}
		case message::RadioEvent::Action::Play:
		{
			sender.play();
			break;
		}
		case message::RadioEvent::Action::Stop:
		{
			sender.stop();
			break;
		}
		default:
		{
			throw std::runtime_error("RadioEvent with incompatible command found in dmp_client");
		}
	}
}
