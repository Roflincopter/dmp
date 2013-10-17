#pragma once

#include <sstream>
#include <memory>
#include <cstdint>

#include <boost/asio.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

#include "message_serializer.hpp"
#include "message.hpp"

namespace dmp {

using namespace boost::asio::ip;

class Connection {
    std::shared_ptr<boost::asio::io_service> io_service;
    tcp::socket socket;

public:
    Connection(std::shared_ptr<boost::asio::io_service> io_service, tcp::socket&& socket)
    : io_service(io_service)
    , socket(std::move(socket))
    {}

    Connection(Connection&& that)
    : io_service(std::move(that.io_service))
    , socket(std::move(that.socket))
    {}

    ~Connection()
    {
        socket.close();
    }

    template <typename T>
    void send(T x)
    {
        std::ostringstream oss;
        boost::archive::text_oarchive oar(oss);
        message::serialize(oar, x);
        std::string content = oss.str();
        uint32_t size = content.size();

        boost::asio::write(socket, boost::asio::buffer(&x.type, 4));
        boost::asio::write(socket, boost::asio::buffer(&size, 4));
        boost::asio::write(socket, boost::asio::buffer(content));
    }

    message::Type receive_type()
    {
        message::Type type = message::Type::NoMessage;

        boost::system::error_code ec;
        std::vector<uint8_t> buf(4);
        size_t read_bytes = boost::asio::read(socket, boost::asio::buffer(buf), ec);
        assert(read_bytes == 4);
        const unsigned char* data = buf.data();
        type = static_cast<message::Type>(*reinterpret_cast<const uint32_t*>(data));
        return type;
    }

    template <typename T>
    T receive()
    {
        uint32_t size;
        std::string content;

        {
            std::vector<uint8_t> buf(4);
            size_t read_bytes = boost::asio::read(socket, boost::asio::buffer(buf));
            assert(read_bytes == 4);
            const unsigned char* data = buf.data();
            size = *reinterpret_cast<const uint32_t*>(data);
        }
        {
            std::vector<uint8_t> buf(size);
            size_t read_bytes = boost::asio::read(socket, boost::asio::buffer(buf));
            assert(read_bytes == size);
            const unsigned char* data = buf.data();
            content = std::string(reinterpret_cast<const char*>(data), size);
        }
        
        std::istringstream iss(content);
        boost::archive::text_iarchive iar(iss);

        T t;
        message::serialize(iar, t);

        return t;
    }
};

}
