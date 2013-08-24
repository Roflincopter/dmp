#include "accept.hpp"

#include <memory>

namespace dmp {

using namespace boost::asio::ip;

connection accept(uint16_t port)
{
    std::unique_ptr<boost::asio::io_service> io_service(new boost::asio::io_service);
    tcp::socket socket(*io_service);
    tcp::acceptor acceptor(*io_service, tcp::endpoint(tcp::v4(), port));
    acceptor.accept(socket);
    return connection(std::move(io_service), std::move(socket));
}

}