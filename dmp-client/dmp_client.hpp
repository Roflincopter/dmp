#pragma once

#include "connection.hpp"

class DmpClient
{
    std::string name;
    dmp::Connection connection;
    message::Ping last_sent_ping;
    message::DmpCallbacks callbacks;
    dmp_library::Library lib;

public:
    DmpClient(std::string name, dmp::Connection&& conn);

    void handle_request(message::Type t);
    void listen_requests();

    void run();
    void bind_callbacks(message::DmpCallbacks cbs);

    void search(std::string query);
    void index(std::string path);

    void handle_ping(message::Ping ping);
    void handle_name_request(message::NameRequest name_req);
    void handle_pong(message::Pong pong);
    void handle_search_request(message::SearchRequest search_req);
    void handle_search_response(message::SearchResponse search_res);
};
