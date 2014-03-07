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

void DmpServer::timed_debug()
{
	debug_timer.expires_from_now(boost::posix_time::seconds(5));

	auto cb = [this](boost::system::error_code ec){
		if(ec)
		{
			throw std::runtime_error("something went wrong in the server debug timer.");
		}

		//std::cout << connections.size() << std::endl;

		timed_debug();
	};
	debug_timer.async_wait(cb);
}

void DmpServer::run()
{
	timed_debug();

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
		set(message::Type::Queue, std::function<void(message::Queue)>(std::bind(&DmpServer::handle_queue, this, std::placeholders::_1)));

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
			} catch(std::runtime_error e) {
				std::cerr << "Endpoint unexpectedly disconnected with message: " << e.what() << std::endl;
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
		radios[ar.name] = std::make_pair(std::thread(), DmpRadio(ar.name, shared_from_this(), port_pool));

		origin->forward(message::AddRadioResponse(ar.name, true, ""));
		radios[ar.name].first = std::thread(std::bind(&DmpRadio::run, &radios[ar.name].second));
		origin->forward(message::ListenConnectionRequest(radios[ar.name].second.get_receiver_port()));
		
		for(auto connection : connections) {
			connection.second->forward(message::AddRadio(ar.name));
		}
	} else {
		origin->forward(message::AddRadioResponse(ar.name, false, "Radio with name " + ar.name + " already exists"));
	}
}

void DmpServer::handle_queue(message::Queue queue)
{
	radios[queue.radio].second.queue(queue.queuer, queue.owner, queue.entry);
	message::PlaylistUpdate::Action action(message::PlaylistUpdate::Action::Type::Append, 0, 0);
	for(auto& endpoint : connections) {
		endpoint.second->forward(message::PlaylistUpdate(action, queue.radio, radios[queue.radio].second.get_playlist()));
	}
}

void DmpServer::order_stream(std::string client, dmp_library::LibraryEntry entry, uint16_t port)
{
	connections[client]->forward(message::StreamRequest(entry, port));
}
