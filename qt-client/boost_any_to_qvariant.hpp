#pragma once

#include <QVariant>
#include <QMetaType>

#include <boost/fusion/iterator.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/fusion/include/advance.hpp>
#include <boost/fusion/include/size.hpp>

#include <string>

Q_DECLARE_METATYPE(std::string)

template<typename value_type>
typename std::enable_if<std::is_same<value_type, std::string>::value, QVariant>::type 
convert(boost::any const& x)
{
	return QVariant::fromValue<QString>(QString::fromStdString(boost::any_cast<value_type>(x)));
}

template<typename value_type>
typename std::enable_if<!std::is_same<value_type, std::string>::value, QVariant>::type 
convert(boost::any const& x)
{
	return QVariant::fromValue<value_type>(boost::any_cast<value_type>(x));
}

template <typename T, int n, int size>
typename std::enable_if<(n < size), QVariant>::type
convert(boost::any const& x)
{
	typedef boost::fusion::result_of::begin<T> begin;
	typedef boost::fusion::result_of::advance_c<typename begin::type, n> adv_it;
	typedef boost::fusion::result_of::deref<typename adv_it::type> deref;
	typedef typename std::decay<typename deref::type>::type value_type;
	return convert<value_type>(x);
}

template <typename T, int n, int size>
typename std::enable_if<!(n < size), QVariant>::type
convert(boost::any const&)
{
	throw std::runtime_error("This should never happen, An out of bounds index for boost fusion was generated.");
}

template <typename T>
QVariant to_qvariant(boost::any const& x, int index)
{
	#define QVARIANT_CONVERSION(X) \
	case X: \
	{\
		typedef typename boost::fusion::result_of::size<T>::type size_type; \
		return convert<T, X, size_type::value>(x); \
	}

	switch(index)
	{
		QVARIANT_CONVERSION(0);
		QVARIANT_CONVERSION(1);
		QVARIANT_CONVERSION(2);
		QVARIANT_CONVERSION(3);
		QVARIANT_CONVERSION(4);
		QVARIANT_CONVERSION(5);
		QVARIANT_CONVERSION(6);
		QVARIANT_CONVERSION(7);
		QVARIANT_CONVERSION(8);
		QVARIANT_CONVERSION(9);
		QVARIANT_CONVERSION(10);
		default : throw std::runtime_error("Boost any to Qvariant conversion switch was too small.");
	}
	#undef QVARIANT_CONVERSION
}