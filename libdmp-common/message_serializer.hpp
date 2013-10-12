#pragma once

#include <boost/archive/basic_text_iarchive.hpp>
#include <boost/archive/basic_text_oarchive.hpp>

#include <boost/fusion/support.hpp>

namespace message {

struct Serialize
{
    template <typename Archive, typename I, typename E>
    static typename std::enable_if<!std::is_same<I,E>::value, void>::type
    serialize(Archive& ar, I const& it, E const& end)
    {
        ar & boost::fusion::deref(it);
        Serialize::serialize(ar, boost::fusion::advance_c<1>(it), end);
    }

    template <typename Archive, typename I, typename E>
    static typename std::enable_if<std::is_same<I,E>::value, void>::type
    serialize(Archive&, I, E)
    {
        return;
    }
};

template <typename Archive, typename T>
void serialize(Archive& ar, T& x)
{
    Serialize::serialize(ar, boost::fusion::begin(x), boost::fusion::end(x));
}

}
