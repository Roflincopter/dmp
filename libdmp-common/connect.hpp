#pragma once

#include "connection.hpp"

#include <boost/asio.hpp>

namespace dmp {

using namespace boost::asio::ip;

Connection connect(std::string hostname, uint16_t port);

}
