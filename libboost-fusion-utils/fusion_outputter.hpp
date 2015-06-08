#pragma once

#include "friendly_fusion.hpp"

#include <boost/fusion/include/pair.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/vector.hpp>

#include <iostream>
#include <ostream>
#include <vector>
#include <string>
#include <map>

//Workaround for argument dependent lookup
namespace std
{

struct Outputter
{
	template <typename I, typename E>
	static typename std::enable_if<!std::is_same<I,E>::value, std::ostream&>::type
	output(std::ostream& os, I const& it, E const& end)
	{
		os << friendly_fusion::deref(it);
		os << ", ";
		return Outputter::output(os, friendly_fusion::advance_c<1>(it), end);
	}

	template <typename I, typename E>
	static typename std::enable_if<std::is_same<I,E>::value, std::ostream&>::type
	output(std::ostream& os, I const&, E const&)
	{
		return os;
	}
};

template <typename T, typename U>
std::ostream& operator<<(std::ostream& os, std::map<T, U> map)
{
	os << "{" << std::endl;
	for (auto&& x : map) {
		os << "\t{" << x.first << ", " << x.second << "}" << std::endl;
	}
	os << "}" << std::endl;
	return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T> vec)
{
	os << "{";
	for (auto&& x : vec) {
		os << "\t" << x;
	}
	os << "}" << std::endl;
	return os;
}

template <>
inline std::ostream& operator<<(std::ostream& os, std::vector<uint8_t> vec)
{
	os << "{";
	for (auto&& x : vec) {
		os << static_cast<unsigned int>(x) << ",";
	}
	os << "}" << std::endl;
	return os;
}

template <size_t I>
inline std::ostream& operator<<(std::ostream& os, std::array<uint8_t, I> arr){
	os << "{";
	for(auto&& x : arr) {
		os << static_cast<unsigned int>(x) << ", ";
	}
	os << "}" << std::endl;
	return os;
}

template <typename T>
typename std::enable_if<friendly_fusion::traits::is_sequence<T>::value, std::ostream&>::type
operator<<(std::ostream& os, T x)
{
	os << "{";
	std::Outputter::output(os, friendly_fusion::begin(x), friendly_fusion::end(x));
	os << "}";
	return os;
}

}


