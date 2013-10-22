#pragma once

#include "connection.hpp"

class DmpServer
{
    std::map<std::string, dmp::Connection> connections;

public:
    DmpServer();

    void add_connection(dmp::Connection&& c);
};
