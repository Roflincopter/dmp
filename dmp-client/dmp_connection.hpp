#pragma once

#include <boost/asio.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>

using namespace boost::asio::ip;

struct dmp_connection {
    boost::asio::io_service io_service;
    basic_resolver<tcp> resolver;
    basic_resolver_query<tcp> query;
    basic_endpoint<tcp> endpoint;
    tcp::socket socket;

    dmp_connection(std::string hostname, uint16_t port)
    : io_service()
    , resolver(io_service)
    , query(hostname, std::to_string(port))
    , endpoint(*resolver.resolve(query))
    , socket(io_service)
    {
        socket.connect(endpoint);
    }

    ~dmp_connection()
    {
        socket.close();
    }

    template <typename T>
    void send(T const& message)
    {
        std::ostringstream oss;
        boost::archive::text_oarchive oar(oss);
        oar & message;

        std::string data = oss.str();
        boost::asio::write(socket, boost::asio::buffer(data));
    }
};