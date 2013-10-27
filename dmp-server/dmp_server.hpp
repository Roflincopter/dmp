#pragma once

#include "connection.hpp"
#include "client_endpoint.hpp"

class DmpServer
{
    boost::asio::io_service server_io_service;
    std::map<std::string, std::unique_ptr<ClientEndpoint>> connections;

    void keep_alive();

public:
    DmpServer();

    void run();
    void add_connection(dmp::Connection&& c);
};
