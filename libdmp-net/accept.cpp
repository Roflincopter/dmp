#include "accept.hpp"

#include <boost/bind.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <memory>

using tcp = boost::asio::ip::tcp;
using io_service = boost::asio::io_service;

void accept_connection(tcp::acceptor& acceptor, std::shared_ptr<io_service> ios, std::shared_ptr<tcp::socket> socket, boost::system::error_code ec, std::function<void(Connection&&)> f)
{
	if(bool(socket) && bool(ios) && !bool(ec))
	{
		f(Connection(ios, std::move(*socket)));
	}

	std::shared_ptr<io_service> new_ios = std::make_shared<io_service>();
	std::shared_ptr<tcp::socket> new_socket = std::make_shared<tcp::socket>(*new_ios);

	acceptor.async_accept(*new_socket, boost::bind(accept_connection, boost::ref(acceptor), new_ios, new_socket, boost::asio::placeholders::error, f));
}

void accept_loop(uint16_t port, io_service& ios, std::function<void(Connection&&)> f)
{
	tcp::acceptor acceptor(ios, tcp::endpoint(tcp::v4(), port));

	accept_connection(acceptor, nullptr, nullptr, boost::system::error_code(), f);
	ios.run();
}
