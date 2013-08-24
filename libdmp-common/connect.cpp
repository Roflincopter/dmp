#include "connect.hpp"

namespace dmp {

connection connect(std::string hostname, uint16_t port)
{
    std::unique_ptr<boost::asio::io_service> io_service(new boost::asio::io_service);
    basic_resolver<tcp> resolver(*io_service);
    basic_resolver_query<tcp> query(hostname, std::to_string(port));
    basic_endpoint<tcp> endpoint;
    tcp::socket socket(*io_service);

    for(tcp::resolver::iterator it = resolver.resolve(query);
        it != tcp::resolver::iterator();
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
            return connection(std::move(io_service), std::move(socket));
        }
    }

    throw std::runtime_error("None of the supplied endpoints for query " + hostname + ":" + std::to_string(port) + " accepted the connection.");
}

}