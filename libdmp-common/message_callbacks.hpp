#pragma once

#include "message.hpp"

#include <boost/variant.hpp>

#include <stdexcept>

namespace message {

template <typename T> using CB = std::function<void(T)>;

using CallBackType = boost::variant<
                                          CB<message::Ping>
                                        , CB<message::Pong>
                                        , CB<message::NameRequest>
                                        , CB<message::NameResponse>
                                        , CB<message::SearchRequest>
                                        , CB<message::SearchResponse>
                                        , CB<message::Bye>
                                        , CB<message::ByeAck>
                                    >;

struct DmpCallbacks {
    std::map<message::Type, CallBackType> callbacks;
    std::function<void()> refresher;

    DmpCallbacks(std::function<void()> refresher)
    : callbacks()
    , refresher(refresher)
    {}

    template<typename argument>
    struct call_visitor : public boost::static_visitor<void>
    {
        argument x;

        call_visitor(argument x)
        : x(x)
        {}

        void operator()(std::function<void(argument)> v)
        {
            v(x);
        }

        template <typename U>
        void operator()(U u)
        {
            throw std::runtime_error("Failed to match right callback");
        }
    };

    template <typename T>
    void operator()(T message) const
    {
        call_visitor<T> v(message);
        auto it = callbacks.find(message.type);

        if (it != callbacks.cend()) {
            boost::apply_visitor(v, it->second);
            refresher();
        } else {
            throw std::runtime_error("Requested callback type was not found in callbacks: " + std::string(typeid(message).name()) + " " + std::to_string(static_cast<uint32_t>(message.type)));
        }
    }

    template <typename T>
    DmpCallbacks& set(message::Type t, T x)
    {
        callbacks[t] = x;
        return *this;
    }
};

}
