#pragma once

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
                                    >;

struct DmpCallbacks {
    std::map<message::Type, CallBackType> callbacks;

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

        std::stringstream x;
        x << static_cast<uint32_t>(message.type);
        std::string y = x.str();

        for (const auto e : callbacks)
        {
            std::cout << static_cast<uint32_t>(e.first) << std::endl;
        }

        if (it != callbacks.cend()) {
            return boost::apply_visitor(v, it->second);
        } else {
            throw std::runtime_error("Requested callback type was not found in callbacks: " + std::string(typeid(message).name()) + y);
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
