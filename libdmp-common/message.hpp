#pragma once

#include <boost/fusion/adapted/struct/adapt_struct.hpp>

#include <cstdint>

namespace message {

enum class Type : uint32_t {
    NoMessage,
    Ping,
    Pong
};

struct Ping {
    
    Type type;
    std::string ping;
    std::string payload;

    struct RandDevice
    {
        RandDevice()
        {
            srand(time(nullptr));
        }

        int operator()() const
        {
            return rand();
        }
    };

    std::string random_string()
    {
        static const std::string alphanum =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        static const RandDevice rand;

        std::string ret = "";
        for (int i = 0; i < 12; ++i) {
            ret += alphanum[rand() % (alphanum.size() - 1)];
        }

        return ret;
    }
    
    Ping()
    : type(Type::Ping)
    , ping("Ping: ")
    , payload(random_string())
    {}
};

struct Pong
{
    Type type;
    std::string pong;
    std::string payload;

    Pong(){}
    Pong(message::Ping p)
    : type(Type::Pong)
    , pong("Pong: ")
    , payload(p.payload)
    {}
};

}

BOOST_FUSION_ADAPT_STRUCT(
    message::Ping,
    (message::Type, type)
    (std::string, ping)
    (std::string, payload)
)

BOOST_FUSION_ADAPT_STRUCT(
    message::Pong,
    (message::Type, type)
    (std::string, pong)
    (std::string, payload)
)