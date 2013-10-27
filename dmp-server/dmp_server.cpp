#include "dmp_server.hpp"

#include <boost/thread.hpp>

DmpServer::DmpServer()
: server_io_service()
{}

void DmpServer::run()
{
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

    auto cep = std::unique_ptr<ClientEndpoint>(new ClientEndpoint(std::move(c)));
    connections[name_res.name] = std::move(cep);

    boost::thread(boost::bind(&ClientEndpoint::run, connections[name_res.name].get()));
}
