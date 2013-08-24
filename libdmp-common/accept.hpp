
#include "connection.hpp"

#include <boost/asio.hpp>

namespace dmp{

using namespace boost::asio::ip;

connection accept(uint16_t port);

}