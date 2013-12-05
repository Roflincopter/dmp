#include "connect.hpp"

namespace dmp {

Connection connect(std::string hostname, uint16_t port)
{
    std::shared_ptr<boost::asio::io_service> io_service = std::make_shared<boost::asio::io_service>();
    basic_resolver<tcp> resolver(*io_service);
    std::string portstr;
    {
        std::stringstream ss;
        ss << port;
        portstr = ss.str();
    }

    basic_resolver_query<tcp> query(hostname, portstr);
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
            return Connection(io_service, std::move(socket));
        }
    }

    throw std::runtime_error("None of the supplied endpoints for query " + hostname + ":" /*+ std::to_string(port)*/ + " accepted the connection.");
}

}
