#pragma once

#include <boost/asio.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <istream>
#include <memory>

namespace dmp {

using namespace boost::asio::ip;

class connection {
    std::unique_ptr<boost::asio::io_service> io_service;
    tcp::socket socket;

public:

    connection(std::unique_ptr<boost::asio::io_service>&& io_service,
               tcp::socket&& socket)
    : io_service(std::move(io_service))
    , socket(std::move(socket))
    {}

    connection(connection&& that)
    : io_service(std::move(that.io_service))
    , socket(std::move(that.socket))
    {}

    ~connection(){}

    template <typename T>
    void send(T const& message)
    {
        std::ostringstream oss;
        boost::archive::text_oarchive oar(oss);
        oar & message;

        std::string data = oss.str();
        boost::asio::write(socket, boost::asio::buffer(data));
    }

    template <typename T>
    T receive()
    {
        boost::asio::streambuf buffer;
        try {
            boost::asio::read(socket, buffer, boost::asio::transfer_all());
        }
        catch (boost::system::system_error &e){
            if (e.code() != boost::asio::error::misc_errors::eof)
            {
                std::cerr << e.code().category().name() << std::endl;
                exit(1);
            }
        }
        std::istream iss(&buffer);
        boost::archive::text_iarchive iar(iss);

        T t;
        iar & t;
        return t;
    }
};

}