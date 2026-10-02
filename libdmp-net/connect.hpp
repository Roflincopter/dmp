#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <stdint.h>
#include <memory>
#include <string>

boost::asio::ip::tcp::socket connect(std::string hostname, uint16_t port, std::weak_ptr<boost::asio::io_context> ioc);
