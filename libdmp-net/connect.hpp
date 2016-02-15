#pragma once

#include <boost/asio/ip/tcp.hpp>

#include <stdint.h>
#include <memory>
#include <string>

namespace boost { namespace asio { class io_service; } }

boost::asio::ip::tcp::socket connect(std::string hostname, uint16_t port, std::weak_ptr<boost::asio::io_service> ios);

