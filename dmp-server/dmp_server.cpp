#include "dmp_server.hpp"
#include "message_callbacks.hpp"

#include <boost/thread.hpp>

#include "message_outputter.hpp"

DmpServer::DmpServer()
: server_io_service()
, connections()
, radios()
, debug_timer(server_io_service)
, port_pool(std::make_shared<NumberPool>(50000, 51000))
{
	gst_init(0, nullptr);
}

void DmpServer::run()
{
	server_io_service.run();
}

void DmpServer::stop()
{
	server_io_service.stop();
}

void DmpServer::add_connection(dmp::Connection&& c)
{
	message::NameRequest name_req;
	c.send(name_req);
	message::Type t = c.receive_type();
	if(t != message::Type::NameResponse) {
		return;
	}
	message::NameResponse name_res = c.receive();

	auto cep = std::make_shared<ClientEndpoint>(name_res.name, std::move(c));

	cep->get_callbacks().
		set(message::Type::SearchRequest, std::function<void(message::SearchRequest)>(std::bind(&DmpServer::handle_search, this, cep, std::placeholders::_1))).
		set(message::Type::AddRadio, std::function<void(message::AddRadio)>(std::bind(&DmpServer::handle_add_radio, this, cep, std::placeholders::_1))).
		set(message::Type::Queue, std::function<void(message::Queue)>(std::bind(&DmpServer::handle_queue, this, std::placeholders::_1))).
		set(message::Type::RadioAction, std::function<void(message::RadioAction)>(std::bind(&DmpServer::handle_radio_action, this, std::placeholders::_1))).
		set(message::Type::TuneIn, std::function<void(message::TuneIn)>(std::bind(&DmpServer::handle_tune_in, this, cep, std::placeholders::_1)));
		
	connections[name_res.name] = cep;

	std::map<std::string, Playlist> playlists;
	for(auto& radio : radios) {
		playlists.emplace(radio.first, radio.second.second.get_playlist());
	}
	connections[name_res.name]->forward(message::Radios(playlists));

	std::thread execthread = std::thread(
		[this, name_res]()
		{
			try {
				connections[name_res.name]->run();
			} catch(std::exception &e) {
				std::cerr << "Endpoint unexpectedly raised exception: " << e.what() << std::endl;
			}
			connections.erase(name_res.name);
		}
	);
	execthread.detach();
}

void DmpServer::handle_search(std::shared_ptr<ClientEndpoint> origin, message::SearchRequest sr)
{
	std::function<void(message::SearchResponse)> cb = std::bind(&ClientEndpoint::forward<message::SearchResponse>, origin.get(), std::placeholders::_1);
	for(auto const& x : connections)
	{
		x.second->search(cb, sr);
	}
}

void DmpServer::handle_add_radio(std::shared_ptr<ClientEndpoint> origin, message::AddRadio ar)
{
	if(radios.find(ar.name) == radios.end()) {
		auto radio_it = radios.emplace(
			std::make_pair(
				ar.name, 
				std::make_pair(
					std::thread(), 
					DmpRadio(
						ar.name, 
						shared_from_this(),
						port_pool
					)
				)
			)
		).first;
		origin->forward(message::AddRadioResponse(ar.name, true, ""));
		radio_it->second.first = std::thread(std::bind(&DmpRadio::run_loop, std::ref(radio_it->second.second)));
		
		radio_it->second.second.listen();
		
		for(auto connection : connections) {
			connection.second->forward(message::AddRadio(ar.name));
		}
	} else {
		origin->forward(message::AddRadioResponse(ar.name, false, "Radio with name " + ar.name + " already exists"));
	}
}

void DmpServer::handle_queue(message::Queue queue)
{
	radios.at(queue.radio).second.queue(queue.queuer, queue.owner, queue.entry);
	message::PlaylistUpdate::Action action(message::PlaylistUpdate::Action::Type::Update, 0, 0);
	for(auto& endpoint : connections) {
		endpoint.second->forward(message::PlaylistUpdate(action, queue.radio, radios.at(queue.radio).second.get_playlist()));
	}
}

void DmpServer::update_playlist(std::string radio_name, Playlist playlist)
{
	message::PlaylistUpdate::Action action(message::PlaylistUpdate::Action::Type::Update, 0, 0);
	for(auto& endpoint : connections) {
		endpoint.second->forward(message::PlaylistUpdate(action, radio_name, playlist));
	}
}

void DmpServer::handle_radio_action(message::RadioAction ra)
{
	auto& radio = radios.at(ra.radio_name);
	switch(ra.action)
	{
		case message::PlaybackAction::Next:
		{
			DEBUG_COUT << "Next called on radio: " << ra.radio_name << std::endl;
			radio.second.next();
			break;
		}
		case message::PlaybackAction::Pause:
		{
			radio.second.pause();
			break;
		}
		case message::PlaybackAction::Play:
		{
			radio.second.play();
			break;
		}
		case message::PlaybackAction::Stop:
		{
			radio.second.stop();
			break;
		}
		//explicit falltrough.
		case message::PlaybackAction::Listen:
		case message::PlaybackAction::NoAction:
		case message::PlaybackAction::Reset:
		default:
		{
			throw std::runtime_error("Unknown RadioAction was passed to the Server, action was: " + std::to_string(static_cast<message::Type_t>(ra.action)));
		}
		
	}
}

void DmpServer::handle_tune_in(std::shared_ptr<ClientEndpoint> origin, message::TuneIn ti)
{
	auto& radio = radios.at(ti.radio_name);
	try {
		radio.second.add_listener(origin->get_name());
	} catch(std::runtime_error const& e) {
		std::cout << e.what() << std::endl;
		return;
	}
	origin->forward(message::ListenConnectionRequest(ti.radio_name, radio.second.get_receiver_port(origin->get_name())));
}

void DmpServer::order_stream(std::string client, std::string radio_name, dmp_library::LibraryEntry entry, uint16_t port)
{
	connections[client]->forward(message::StreamRequest(radio_name, entry, port));
}

void DmpServer::forward_receiver_action(std::string client, message::ReceiverAction ra)
{
	connections[client]->forward(ra);
}

void DmpServer::forward_sender_action(std::string client, message::SenderAction sa)
{
	connections[client]->forward(sa);
}
