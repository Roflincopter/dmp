#pragma once

#include "dmp_radio.hpp"
#include "client_endpoint.hpp"

#include "connection.hpp"
#include "message_callbacks.hpp"

class DmpServer
{
    boost::asio::io_service server_io_service;
    std::map<std::string, std::shared_ptr<ClientEndpoint>> connections;
    message::DmpCallbacks callbacks;
    std::map<std::string, std::map<std::string, message::SearchResponse>> temporary_responses;
    std::map<std::string, DmpRadio> radios;

public:
    DmpServer();

    void run();
    void add_connection(dmp::Connection&& c);

    void handle_search(std::shared_ptr<ClientEndpoint> origin, message::SearchRequest sr);
};
