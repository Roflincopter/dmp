
#pragma once

#include "connection.hpp"
#include "dmp_sender.hpp"
#include "dmp_receiver.hpp"

class DmpClient
{
    std::string name;
    dmp::Connection connection;
    message::Ping last_sent_ping;
    dmp_library::Library lib;

    DmpSender sender;
    DmpReceiver receiver;

    message::DmpCallbacks callbacks;

public:

    DmpClient(std::string name, dmp::Connection&& conn);
    DmpClient(DmpClient&&) = default;

    message::DmpCallbacks& get_callbacks(){return callbacks;}

    void handle_request(message::Type t);
    void listen_requests();

    void stop(){connection.io_service->stop();}
    void run();
    void bind_callbacks(message::DmpCallbacks cbs);

    void search(std::string query);
    void index(std::string path);
    void send_bye();

    void handle_ping(message::Ping ping);
    void handle_name_request(message::NameRequest name_req);
    void handle_pong(message::Pong pong);
    void handle_search_request(message::SearchRequest search_req);
    void handle_search_response(message::SearchResponse search_res);
};
