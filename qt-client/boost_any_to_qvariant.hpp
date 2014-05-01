#pragma once

#include "friendly_fusion.hpp"
#include "index_list.hpp"
#include "dmp_qt_meta_types.hpp"

#include <QVariant>

#include <boost/any.hpp>

#include <string>

template<typename value_type>
QVariant convert(boost::any const& x)
{
	return QVariant::fromValue<value_type>(boost::any_cast<value_type>(x));
}

template <typename T, int n>
QVariant convert(boost::any const& x)
{
	typedef friendly_fusion::result_of::begin<T> begin;
	typedef friendly_fusion::result_of::advance_c<typename begin::type, n> adv_it;
	typedef friendly_fusion::result_of::deref<typename adv_it::type> deref;
	typedef typename std::decay<typename deref::type>::type value_type;
	return convert<value_type>(x);
}

#if defined( __GNUC__ ) && !defined( __clang__ )
template <typename T, int index>
std::function<QVariant(boost::any const&)> convert_lambda()
{
	return [](boost::any const& any)
	{
		return convert<T, index>(any);
	};
}

#endif //defined( __GNUC__ ) && !defined( __clang__ )

template<typename T, int... Indices>
QVariant to_qvariant(boost::any const& any, int index, indices<Indices...>)
{
	typedef std::function<QVariant(boost::any const&)> element_type;
	static element_type table[] =
	{
#if defined( __GNUC__ ) && !defined( __clang__ )
		//Workaround for gcc bug: http://gcc.gnu.org/bugzilla/show_bug.cgi?id=47226
		convert_lambda<T, Indices>()
		...
#else
		[](boost::any const& any){return convert<T, Indices>(any);}
		...
#endif //defined( __GNUC__ ) && !defined( __clang__ )
	};

	return table[index](any);
}


template <typename T>
QVariant to_qvariant(boost::any const& x, int index)
{
	typedef typename friendly_fusion::result_of::size<T>::type seq_size;
	typedef typename build_indices<seq_size::value>::type indices_type;

	return to_qvariant<T>(x, index, indices_type{});
}