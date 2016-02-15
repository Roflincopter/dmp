
#include <boost/asio/ip/tcp.hpp>

#include <stdint.h>
#include <functional>
#include <memory>

void accept_loop(uint16_t port, std::weak_ptr<boost::asio::io_service> ios, std::function<void(boost::asio::ip::tcp::socket&&)> f);

