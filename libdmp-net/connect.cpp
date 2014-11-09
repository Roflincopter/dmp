#include "connect.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/basic_resolver.hpp>
#include <boost/asio/ip/basic_resolver_query.hpp>
#include <boost/asio/ip/basic_resolver_iterator.hpp>
#include <boost/asio/ip/tcp.hpp>

using tcp = boost::asio::ip::tcp;
using io_service = boost::asio::io_service;

Connection connect(std::string hostname, uint16_t port, std::weak_ptr<boost::asio::io_service> ios)
{
	boost::asio::ip::basic_resolver<tcp> resolver(*ios.lock());
	std::string portstr;
	{
		std::stringstream ss;
		ss << port;
		portstr = ss.str();
	}

	boost::asio::ip::basic_resolver_query<tcp> query(hostname, portstr);
	boost::asio::ip::basic_endpoint<tcp> endpoint;
	boost::asio::ip::tcp::socket socket(*ios.lock());

	for(boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);
		it != boost::asio::ip::tcp::resolver::iterator();
		++it)
	{
		endpoint = *it;
		boost::system::error_code ec;
		socket.connect(endpoint, ec);
		if(ec)
		{
			continue;
		}
		else
		{
			return Connection(std::move(socket));
		}
	}

	throw std::runtime_error("None of the supplied endpoints for query " + hostname + ":" /*+ std::to_string(port)*/ + " accepted the connection.");
}
