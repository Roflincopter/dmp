#include "accept.hpp"

#include "connection.hpp"

#include <boost/asio/basic_socket_acceptor.hpp>
#include <boost/asio/basic_stream_socket.hpp>
#include <boost/asio/detail/impl/reactive_socket_service_base.ipp>
#include <boost/asio/detail/impl/service_registry.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/stream_socket_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/bind/bind.hpp>

#include <boost/system/error_code.hpp>

#include <new>
#include <memory>
#include <algorithm>


using tcp = boost::asio::ip::tcp;
using io_service = boost::asio::io_service;

void accept_connection(std::shared_ptr<tcp::acceptor> acceptor, std::weak_ptr<io_service> ios, std::shared_ptr<tcp::socket> socket, boost::system::error_code ec, std::function<void(boost::asio::ip::tcp::socket&&)> f)
{
	if(!bool(ec))
	{
		f(std::move(*socket));
		std::shared_ptr<tcp::socket> new_socket = std::make_shared<tcp::socket>(*ios.lock());
		acceptor->async_accept(*new_socket, boost::bind(accept_connection, acceptor, ios, new_socket, boost::asio::placeholders::error, f));
	}
}

void accept_loop(uint16_t port, std::weak_ptr<io_service> ios, std::function<void(boost::asio::ip::tcp::socket&&)> f)
{
	auto acceptor = std::make_shared<tcp::acceptor>(*ios.lock(), tcp::endpoint(tcp::v4(), port));

	std::shared_ptr<tcp::socket> new_socket = std::make_shared<tcp::socket>(*ios.lock());

	acceptor->async_accept(*new_socket, boost::bind(accept_connection, acceptor, ios, new_socket, boost::asio::placeholders::error, f));
}
