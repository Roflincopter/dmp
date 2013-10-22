#include "dmp_server.hpp"

DmpServer::DmpServer()
{
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
    //connections.emplace(std::make_pair(name_res.name, std::move(c)));
}
