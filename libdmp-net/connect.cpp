#include "connect.hpp"

#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/system/error_code.hpp>

#include <stdexcept>
#include <string>

using tcp = boost::asio::ip::tcp;

tcp::socket connect(std::string hostname, uint16_t port, std::weak_ptr<boost::asio::io_context> ioc)
{
	auto context = ioc.lock();
	tcp::resolver resolver(*context);
	auto endpoints = resolver.resolve(hostname, std::to_string(port));

	tcp::socket socket(*context);
	boost::system::error_code ec;
	boost::asio::connect(socket, endpoints, ec);
	if(ec)
	{
		throw std::runtime_error("None of the supplied endpoints for query " + hostname + ":" + std::to_string(port) + " accepted the connection.");
	}

	return socket;
}
