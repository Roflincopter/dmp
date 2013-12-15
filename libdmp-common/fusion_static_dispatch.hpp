#pragma once

#include <boost/fusion/sequence.hpp>
#include <boost/fusion/adapted.hpp>
#include <boost/fusion/sequence/intrinsic/size.hpp>
#include <boost/fusion/include/size.hpp>

#include <string>

namespace std {
	std::string to_string(std::string x);
}

template<int... Indices>
struct indices {
	typedef indices<Indices..., sizeof...(Indices)> next;
};

template<int N>
struct build_indices {
	typedef typename build_indices<N - 1>::type::next type;
};

template<>
struct build_indices<0> {
	typedef indices<> type;
};

template<typename T, int... Indices>
std::string get_nth(T seq, int index, indices<Indices...>)
{
	typedef std::string(*element_type)(T);
	static element_type table[] = {
		[](T seq){return std::to_string(boost::fusion::at_c<Indices>(seq));}
		...
	};

	return table[index](seq);
}

template<typename T>
std::string get_nth(T seq, int index)
{
	typedef typename boost::fusion::result_of::size<T>::type seq_size;
	typedef typename build_indices<seq_size::value>::type indices_type;

	return get_nth(seq, index, indices_type{});
}

template<typename T, int... Indices>
std::string get_nth_name(int index, indices<Indices...>)
{
	typedef std::string(*element_type)();
	static element_type table[] = {
		[]{return std::string(boost::fusion::extension::struct_member_name<T, Indices>::call());}
		...
	};

	return table[index]();
}

template <typename T>
std::string get_nth_name(int index)
{
	typedef typename boost::fusion::result_of::size<T>::type seq_size;
	typedef typename build_indices<seq_size::value>::type indices_type;

	return get_nth_name<T>(index, indices_type{});
}
