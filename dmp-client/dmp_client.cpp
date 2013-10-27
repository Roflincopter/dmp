#include "dmp_client.hpp"

DmpClient::DmpClient(std::string name, dmp::Connection&& conn)
: name(name)
, connection(std::move(conn))
{
    listen_requests();
    listen_input();
    connection.io_service->run();
}

void DmpClient::handle_request(message::Type t)
{
    std::cerr << "Received Type: " << static_cast<uint32_t>(t) << std::endl;
    switch(t)
    {
        case message::Type::NoMessage:
        {
            throw std::runtime_error("We received a None message this shouldn't happen");
            break;
        }
        case message::Type::Ping:
        {
            connection.async_receive<message::Ping>([this](message::Ping m){handle_ping(m);});
            break;
        }
        case message::Type::Pong:
        {
            connection.async_receive<message::Pong>([this](message::Pong m){handle_pong(m);});
            break;
        }
        case message::Type::NameRequest: {
            connection.async_receive<message::NameRequest>([this](message::NameRequest m){handle_name_request(m);});
            break;
        }
        case message::Type::NameResponse: {
            throw std::runtime_error("Client shouldn't receive a NameResponse ever.");
            break;
        }
        default:
        {
            listen_requests();
        }
    }
}

void DmpClient::listen_requests()
{
    std::function<void(message::Type)> cb = std::bind(&DmpClient::handle_request, this, std::placeholders::_1);
    connection.async_receive_type(cb);
}

void DmpClient::listen_input()
{

}

void DmpClient::handle_ping(message::Ping ping)
{
    message::Pong pong(ping);
    connection.send(pong);

    listen_requests();
}

void DmpClient::handle_pong(message::Pong pong)
{
    assert(last_sent_ping.payload != "");
    if (last_sent_ping.payload != pong.payload) {
        connection.io_service->stop(); //something went wrong terminate connection.
    }

    listen_requests();
}

void DmpClient::handle_name_request(message::NameRequest name_req)
{
    message::NameResponse name_res(name);
    connection.send(name_res);

    listen_requests();
}
