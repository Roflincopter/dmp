#pragma once

#include <stdint.h>
#include <tuple>

namespace boost {
namespace serialization {

template<uint32_t N>
struct Serialize
{
	template<class Archive, typename... Args>
	static void serialize(Archive & ar, std::tuple<Args...> & t, const unsigned int version)
	{
		ar & std::get<N-1>(t);
		Serialize<N-1>::serialize(ar, t, version);
	}
};

template<>
struct Serialize<0>
{
	template<class Archive, typename... Args>
	static void serialize(Archive& __attribute__((unused)) ar, std::tuple<Args...>& __attribute__((unused)) t, const unsigned int __attribute__((unused)) version)
	{
	}
};

template<class Archive, typename... Args>
void serialize(Archive & ar, std::tuple<Args...> & t, const unsigned int version)
{
	Serialize<sizeof...(Args)>::serialize(ar, t, version);
}

}
}
