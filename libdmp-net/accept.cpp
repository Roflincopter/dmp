#include "accept.hpp"

#include <boost/bind.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <memory>

using tcp = boost::asio::ip::tcp;
using io_service = boost::asio::io_service;

void accept_connection(std::shared_ptr<tcp::acceptor> acceptor, std::weak_ptr<io_service> ios, std::shared_ptr<tcp::socket> socket, boost::system::error_code ec, std::function<void(Connection&&)> f)
{
	if(!bool(ec))
	{
		f(Connection(std::move(*socket)));
		std::shared_ptr<tcp::socket> new_socket = std::make_shared<tcp::socket>(*ios.lock());
		acceptor->async_accept(*new_socket, boost::bind(accept_connection, acceptor, ios, new_socket, boost::asio::placeholders::error, f));
	}
}

void accept_loop(uint16_t port, std::weak_ptr<io_service> ios, std::function<void(Connection&&)> f)
{
	auto acceptor = std::make_shared<tcp::acceptor>(*ios.lock(), tcp::endpoint(tcp::v4(), port));

	std::shared_ptr<tcp::socket> new_socket = std::make_shared<tcp::socket>(*ios.lock());

	acceptor->async_accept(*new_socket, boost::bind(accept_connection, acceptor, ios, new_socket, boost::asio::placeholders::error, f));
}
