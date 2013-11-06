#include "client_endpoint.hpp"
#include "message.hpp"

ClientEndpoint::ClientEndpoint(std::string name, dmp::Connection&& conn)
: name(name)
, ping_timer(new boost::asio::deadline_timer(*conn.io_service))
, connection(std::move(conn))
, last_ping()
{
    listen_requests();
    keep_alive();
}

void ClientEndpoint::run()
{
    connection.io_service->run();
}

void ClientEndpoint::bind_callbacks(message::DmpCallbacks cbs)
{
    callbacks = cbs;
}

void ClientEndpoint::search(std::function<void(message::SearchResponse)> cb, message::SearchRequest sr)
{
    callbacks.set(message::Type::SearchResponse, cb);
    connection.send(sr);
}

void ClientEndpoint::handle_request(message::Type t)
{
    switch(t)
    {
        /*case message::Type::NoMessage: {
            throw std::runtime_error("We received a None message this shouldn't happen");
            break;
        }
        case message::Type::Ping: {
            connection.async_receive<message::Ping>([this](message::Ping m){handle_ping(m);});
            break;
        }*/
        case message::Type::Pong: {
            connection.async_receive<message::Pong>(callbacks);
            break;
        }
        case message::Type::SearchRequest: {
            connection.async_receive<message::SearchRequest>(callbacks);
            break;
        }
        case message::Type::SearchResponse: {
            connection.async_receive<message::SearchResponse>(callbacks);
            break;
        }
        /*
        case message::Type::NameRequest: {
            connection.async_receive<message::NameRequest>([this](message::NameRequest m){handle_name_request(m);});
            break;
        }
        case message::Type::NameResponse: {
            throw std::runtime_error("Client shouldn't receive a NameResponse ever.");
            break;
        }*/
        default: {
            listen_requests();
        }
    }
}

void ClientEndpoint::handle_pong(message::Pong p)
{
    if(p.payload != last_ping.payload) {
        connection.io_service->stop();
    }

    listen_requests();
}

void ClientEndpoint::listen_requests()
{
    std::function<void(message::Type)> cb = std::bind(&ClientEndpoint::handle_request, this, std::placeholders::_1);
    connection.async_receive_type(cb);
}

void ClientEndpoint::keep_alive()
{
    ping_timer->expires_from_now(boost::posix_time::seconds(10));
    auto cb = [this](boost::system::error_code const& ec)
    {
        if(ec)
        {
            throw std::runtime_error("something went wrong in the keep alive timer.");
        }

        last_ping = message::Ping();
        connection.send(last_ping);
        keep_alive();
    };
    ping_timer->async_wait(cb);
}
