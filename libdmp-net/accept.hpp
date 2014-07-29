
#include "connection.hpp"

#include <boost/asio/io_service.hpp>

void accept_loop(uint16_t port, boost::asio::io_service& io_service, std::function<void(Connection&&)> f);

