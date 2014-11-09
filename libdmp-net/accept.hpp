
#include "connection.hpp"

#include <boost/asio/io_service.hpp>

void accept_loop(uint16_t port, std::weak_ptr<boost::asio::io_service> ios, std::function<void(Connection&&)> f);

