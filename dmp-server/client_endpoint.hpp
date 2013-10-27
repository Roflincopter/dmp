#pragma once

#include "connection.hpp"

#include <boost/asio/deadline_timer.hpp>

class ClientEndpoint
{
    std::unique_ptr<boost::asio::deadline_timer> ping_timer;
    dmp::Connection connection;
    message::Ping last_ping;

public:

    ClientEndpoint(dmp::Connection&& conn);

    void run();
    void keep_alive();
    void listen_requests();

    void handle_request(message::Type t);
    void handle_pong(message::Pong p);
};
