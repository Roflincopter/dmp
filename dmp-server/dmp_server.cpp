#include "dmp_server.hpp"
#include "message_callbacks.hpp"

#include <boost/thread.hpp>

DmpServer::DmpServer()
: server_io_service()
, connections()
, radios()
, debug_timer(server_io_service)
{}

void DmpServer::timed_debug()
{
	debug_timer.expires_from_now(boost::posix_time::seconds(5));

	auto cb = [this](boost::system::error_code ec){
		if(ec)
		{
			throw std::runtime_error("something went wrong in the server debug timer.");
		}
		timed_debug();
	};
	debug_timer.async_wait(cb);
}

void DmpServer::run()
{
	timed_debug();

	server_io_service.run();
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

	cep->get_callbacks().set(message::Type::Pong, std::function<void(message::Pong)>(std::bind(&ClientEndpoint::handle_pong, cep.get(), std::placeholders::_1)))
		.set(message::Type::SearchRequest, std::function<void(message::SearchRequest)>(std::bind(&DmpServer::handle_search, this, cep, std::placeholders::_1)));

	connections[name_res.name] = std::move(cep);

	boost::thread* execthread = new boost::thread(boost::bind(&ClientEndpoint::run, connections[name_res.name].get()));
	boost::thread jointhread([this, execthread, name_res](){execthread->join(); connections.erase(name_res.name); delete execthread;});
	jointhread.detach();
}

void DmpServer::handle_search(std::shared_ptr<ClientEndpoint> origin, message::SearchRequest sr)
{
	std::function<void(std::shared_ptr<ClientEndpoint>, message::SearchResponse)> cb1 = std::bind(&ClientEndpoint::forward<message::SearchResponse>, origin.get(), std::placeholders::_1, std::placeholders::_2);
	for(auto const& x : connections)
	{
		std::function<void(message::SearchResponse)> cb2 = std::bind(cb1, x.second, std::placeholders::_1);
		x.second->search(cb2, sr);
	}
}
