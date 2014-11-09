#pragma once

#include "connection.hpp"

Connection connect(std::string hostname, uint16_t port, std::weak_ptr<boost::asio::io_service> ios);

