#include "accept.hpp"

#include <functional>
#include <memory>
#include <boost/thread.hpp>


namespace dmp {

using namespace boost::asio::ip;

void accept_connection(tcp::acceptor& acceptor, std::shared_ptr<boost::asio::io_service> io_service, std::shared_ptr<tcp::socket> socket, boost::system::error_code ec, std::function<void(dmp::Connection&&)> f)
{
	if(bool(socket) && bool(io_service) && !bool(ec))
	{
		f(dmp::Connection(io_service, std::move(*socket)));
	}

	std::shared_ptr<boost::asio::io_service> new_io_service = std::make_shared<boost::asio::io_service>();
	std::shared_ptr<tcp::socket> new_socket = std::make_shared<tcp::socket>(*new_io_service);

	acceptor.async_accept(*new_socket, boost::bind(accept_connection, boost::ref(acceptor), new_io_service, new_socket, boost::asio::placeholders::error, f));
}

void accept_loop(uint16_t port, boost::asio::io_service &io_service, std::function<void(dmp::Connection&&)> f)
{
	tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));

	accept_connection(acceptor, nullptr, nullptr, boost::system::error_code(), f);
	io_service.run();
}

}
