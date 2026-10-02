#include "accept.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/system/error_code.hpp>

#include <memory>
#include <utility>

using tcp = boost::asio::ip::tcp;

namespace {

void accept_next(std::shared_ptr<tcp::acceptor> acceptor, std::function<void(tcp::socket&&)> f)
{
	acceptor->async_accept([acceptor, f](boost::system::error_code ec, tcp::socket socket) {
		if(!ec)
		{
			f(std::move(socket));
			accept_next(acceptor, f);
		}
	});
}

}

void accept_loop(uint16_t port, std::weak_ptr<boost::asio::io_context> ioc, std::function<void(tcp::socket&&)> f)
{
	auto acceptor = std::make_shared<tcp::acceptor>(*ioc.lock(), tcp::endpoint(tcp::v4(), port));
	accept_next(acceptor, f);
}
