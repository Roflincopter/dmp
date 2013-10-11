#pragma once

#include <boost/archive/basic_text_iarchive.hpp>
#include <boost/archive/basic_text_oarchive.hpp>

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/support.hpp>

namespace message {

template <typename Archive, typename T>
struct Serialize
{
    template <typename I, typename E>
    static typename std::enable_if<!std::is_same<I,E>::value, void>::type
    serialize(Archive& ar, I const& it, E const& end)
    {
        ar & boost::fusion::deref(it);
        Serialize::serialize(ar, boost::fusion::advance_c<1>(it), end);
    }

    template <typename I, typename E>
    static typename std::enable_if<std::is_same<I,E>::value, void>::type
    serialize(Archive&, I, E)
    {
        return;
    }

    static void serialize(Archive& ar, T& x)
    {
        serialize(ar, boost::fusion::begin(x), boost::fusion::end(x));
    }
};

template <typename T> using out_serializer = Serialize<boost::archive::text_oarchive, T>;
template <typename T> using in_serializer  = Serialize<boost::archive::text_iarchive, T>;



struct Ping {
    
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

        return "Ping: " + ret;
    }
    
    Ping()
    : payload(random_string())
    {}
};

struct Pong
{
    std::string payload;

    Pong(){}
    Pong(message::Ping p)
    : payload("Pong: " + p.payload.substr(6))
    {}
};

}

BOOST_FUSION_ADAPT_STRUCT(
    message::Ping,
    (std::string, payload)
)

BOOST_FUSION_ADAPT_STRUCT(
    message::Pong,
    (std::string, payload)
)
