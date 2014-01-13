#pragma once

#include "index_list.hpp"

#include <boost/fusion/sequence.hpp>
#include <boost/fusion/adapted.hpp>
#include <boost/fusion/sequence/intrinsic/size.hpp>
#include <boost/fusion/include/size.hpp>

#include <string>

namespace std {
	std::string to_string(std::string x);
}

#ifdef __GNUC__
template<int index, typename T>
std::function<std::string(T)> make_at_c_lambda(T seq)
{
	return [](T seq){
		return std::to_string(boost::fusion::at_c<index>(seq));
	};
}
#endif //__GNUC__

template<typename T, int... Indices>
std::string get_nth(T seq, int index, indices<Indices...>)
{
	typedef std::function<std::string(T)> element_type;
	static element_type table[] =
	{
#ifdef __GNUC__
		//Workaround for gcc bug: http://gcc.gnu.org/bugzilla/show_bug.cgi?id=47226
		make_at_c_lambda<Indices>(seq)
		...
#else
		[](T seq){return std::to_string(boost::fusion::at_c<Indices>(seq));}
		...
#endif //__GNUC__
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

#ifdef __GNUC__
template<int index, typename T>
std::function<std::string()> make_struct_member_name_lambda()
{
	return []{
		return std::string(boost::fusion::extension::struct_member_name<T, index>::call());
	};
}
#endif //__GNUC__

template<typename T, int... Indices>
std::string get_nth_name(int index, indices<Indices...>)
{
	typedef std::function<std::string()> element_type;
	static element_type table[] = {
#ifdef __GNUC__
		//Workaround for gcc bug: http://gcc.gnu.org/bugzilla/show_bug.cgi?id=47226
		make_struct_member_name_lambda<Indices, T>()
		...
#else
		[]{return std::string(boost::fusion::extension::struct_member_name<T, Indices>::call());}
		...
#endif //__GNUC__
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
