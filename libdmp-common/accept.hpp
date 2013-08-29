
#include "connection.hpp"

#include <boost/asio.hpp>

namespace dmp{

using namespace boost::asio::ip;

connection accept(uint16_t port);
void accept_loop(uint16_t port, boost::asio::io_service& io_service);

}