#pragma once

#include "dmp-library.hpp"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

#include <cstdint>

namespace message {

enum class Type : uint32_t {
    NoMessage,
    Ping,
    Pong,
    NameRequest,
    NameResponse,
    SearchRequest,
    SearchResponse,
    Bye,
    ByeAck
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

    Pong()
    : type(Type::Pong)
    , pong("Pong: ")
    , payload("")
    {}

    Pong(message::Ping p)
    : type(Type::Pong)
    , pong("Pong: ")
    , payload(p.payload)
    {}
};

struct NameRequest
{
    Type type;

    NameRequest()
    : type(Type::NameRequest)
    {}

};

struct NameResponse
{
    Type type;
    std::string name;

    NameResponse()
    : type(Type::NameResponse)
    , name("")
    {}

    NameResponse(std::string name)
    : type(Type::NameResponse)
    , name(name)
    {}
};

struct SearchRequest
{
    Type type;
    std::string query;

    SearchRequest()
    : type(Type::SearchRequest)
    , query()
    {}

    SearchRequest(std::string query)
    : type(Type::SearchRequest)
    , query(query)
    {}
};

struct SearchResponse
{
    Type type;
    std::string origin;
    std::string query;
    std::vector<dmp_library::LibraryEntry> results;

    SearchResponse()
    : type(Type::SearchResponse)
    , origin()
    , query()
    , results()
    {}

    SearchResponse(std::string query, std::vector<dmp_library::LibraryEntry> results, std::string origin)
    : type(Type::SearchResponse)
    , origin(origin)
    , query(query)
    , results(results)
    {}
};

struct Bye
{
    Type type;

    Bye()
    : type(Type::Bye)
    {}
};

struct ByeAck
{
    Type type;

    ByeAck()
    : type(Type::ByeAck)
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

BOOST_FUSION_ADAPT_STRUCT(
    message::NameRequest,
    (message::Type, type)
)

BOOST_FUSION_ADAPT_STRUCT(
    message::NameResponse,
    (message::Type, type)
    (std::string, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    message::SearchRequest,
    (message::Type, type)
    (std::string, query)
)


BOOST_FUSION_ADAPT_STRUCT(
    message::SearchResponse,
    (message::Type, type)
    (std::string, origin)
    (std::string, query)
    (std::vector<dmp_library::LibraryEntry>, results)
)

BOOST_FUSION_ADAPT_STRUCT(
    message::Bye,
    (message::Type, type)
)


BOOST_FUSION_ADAPT_STRUCT(
    message::ByeAck,
    (message::Type, type)
)


