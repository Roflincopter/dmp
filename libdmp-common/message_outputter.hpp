#pragma once

#include <boost/fusion/support.hpp>
#include <boost/fusion/adapted.hpp>
#include <boost/fusion/include/pair.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/vector.hpp>

#include <iostream>
#include <ostream>
#include <vector>
#include <string>

//Workaround for argument dependent lookup
namespace std
{

struct Outputter
{
	template <typename I, typename E>
	static typename std::enable_if<!std::is_same<I,E>::value, std::ostream&>::type
	output(std::ostream& os, I const& it, E const& end)
	{
		os << boost::fusion::deref(it);
		return Outputter::output(os, boost::fusion::advance_c<1>(it), end);
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

template <typename T>
typename std::enable_if<boost::fusion::traits::is_sequence<T>::value, std::ostream&>::type
operator<<(std::ostream& os, T x)
{
	return std::Outputter::output(os, boost::fusion::begin(x), boost::fusion::end(x));
}

inline std::ostream& operator<<(std::ostream& os, message::Type t)
{
	return os << static_cast<message::Type_t>(t);
}

}


