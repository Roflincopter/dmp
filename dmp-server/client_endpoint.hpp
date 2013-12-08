#pragma once

#include "connection.hpp"
#include "message_callbacks.hpp"

#include <boost/asio/deadline_timer.hpp>

class ClientEndpoint
{
    std::string name;
    std::unique_ptr<boost::asio::deadline_timer> ping_timer;
    dmp::Connection connection;
    message::Ping last_ping;
    message::DmpCallbacks callbacks;

public:

    ClientEndpoint(std::string name, dmp::Connection&& conn);

    void run();
    void keep_alive();
    void listen_requests();

    std::string get_name() {return name;}

    message::DmpCallbacks& get_callbacks();

    //Warning only callbacks can use this forward function as a callback... need to fix that
    template <typename T>
    void forward(std::shared_ptr<ClientEndpoint> sender, T x)
    {
        connection.send(x);
        //sender->listen_requests();
    }

    void search(std::function<void(message::SearchResponse)> cb, message::SearchRequest sr);

    void handle_request(message::Type t);
    void handle_pong(message::Pong p);
    void handle_bye(message::Bye);
};
