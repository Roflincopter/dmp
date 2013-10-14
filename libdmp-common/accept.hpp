
#include "connection.hpp"

#include <boost/asio.hpp>

namespace dmp{

using namespace boost::asio::ip;

void accept_loop(uint16_t port, boost::asio::io_service& io_service, std::function<void(dmp::connection&&)> f);

}
