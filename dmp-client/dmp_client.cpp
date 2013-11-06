#include "dmp_client.hpp"

DmpClient::DmpClient(std::string name, dmp::Connection&& conn)
: name(name)
, connection(std::move(conn))
, lib()
{
    listen_requests();
}

void DmpClient::run()
{
    connection.io_service->run();
}

void DmpClient::bind_callbacks(message::DmpCallbacks cbs)
{
    callbacks = cbs;
}

void DmpClient::index(std::string path)
{
    lib = dmp_library::create_library(path);
}

void DmpClient::handle_request(message::Type t)
{
    switch(t)
    {
        case message::Type::NoMessage:
        {
            throw std::runtime_error("We received a None message this shouldn't happen");
            break;
        }
        case message::Type::Ping:
        {
            connection.async_receive<message::Ping>(callbacks);
            break;
        }
        case message::Type::Pong:
        {
            connection.async_receive<message::Pong>(callbacks);
            break;
        }
        case message::Type::NameRequest: {
            connection.async_receive<message::NameRequest>(callbacks);
            break;
        }
        case message::Type::NameResponse: {
            throw std::runtime_error("Client shouldn't receive a NameResponse ever.");
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

void DmpClient::search(std::string query)
{
    message::SearchRequest q(query);
    connection.send(q);
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

void DmpClient::handle_search_request(message::SearchRequest search_req)
{
    dmp_library::LibrarySearcher searcher(lib);
    message::SearchResponse response(searcher.search(search_req.query));
    connection.send(response);

    listen_requests();
}

void DmpClient::handle_search_response(message::SearchResponse search_res)
{
    for (auto e : search_res.results) {
        std::cout << e << std::endl;
    }

    listen_requests();
}
