#include "accept.hpp"

#include <memory>
#include <boost/thread.hpp>

namespace dmp {

using namespace boost::asio::ip;

void interrupt_point(boost::system::error_code const& e, boost::asio::deadline_timer& t)
{
    std::cout << "Inserted interruption point" << std::endl;
    boost::this_thread::interruption_point;
    t.expires_from_now(boost::posix_time::seconds(1));
    t.async_wait(boost::bind(interrupt_point, boost::asio::placeholders::error, boost::ref(t)));
}

void accept_connection(tcp::acceptor& acceptor, std::shared_ptr<boost::asio::io_service> io_service, std::shared_ptr<tcp::socket> socket, boost::system::error_code ec)
{
    if(bool(socket) && bool(io_service) && !bool(ec))
    {
        std::cout << "Handling connection: " << std::endl;
        //process connection
        //reset pointer
    }

    std::shared_ptr<boost::asio::io_service> new_io_service = std::make_shared<boost::asio::io_service>();
    std::shared_ptr<tcp::socket> new_socket = std::make_shared<tcp::socket>(*new_io_service);

    acceptor.async_accept(*new_socket, boost::bind(accept_connection, boost::ref(acceptor), new_io_service, new_socket, boost::asio::placeholders::error));
}

void accept_loop(uint16_t port, boost::asio::io_service &io_service)
{
    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));

    accept_connection(acceptor, nullptr, nullptr, boost::system::error_code());
    io_service.run();
}

connection accept(uint16_t port)
{
    std::shared_ptr<boost::asio::io_service> io_service = std::make_shared<boost::asio::io_service>();
    tcp::socket socket(*io_service);
    tcp::acceptor acceptor(*io_service, tcp::endpoint(tcp::v4(), port));
    acceptor.accept(socket);
    return connection(io_service, std::move(socket));
}

}