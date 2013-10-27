#pragma once

#include "connection.hpp"

class DmpClient
{
    std::string name;
    dmp::Connection connection;
    message::Ping last_sent_ping;

public:
    DmpClient(std::string name, dmp::Connection&& conn);

    void handle_request(message::Type t);
    void listen_requests();
    void listen_input();

    void handle_ping(message::Ping ping);
    void handle_name_request(message::NameRequest name_req);
    void handle_pong(message::Pong pong);
};
