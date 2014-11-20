#pragma once

#include "friendly_fusion.hpp"
#include "index_list.hpp"
#include "fusion_static_dispatch.hpp"

#include <QVariant>

#include <boost/any.hpp>

#include <string>

template <typename T>
struct ToQvariantFunctor {
	
	typedef QVariant return_type;
	
	template <int I>
	static return_type call(boost::any const& any)
	{
		typedef typename friendly_fusion::utils::DecayedTypeOfAtIndex<T, I>::type value_type;
		
		return QVariant::fromValue<value_type>(boost::any_cast<value_type>(any));
	}

};

template <typename T>
QVariant to_qvariant(boost::any const& x, int index)
{
	return apply_functor_to_member<T, ToQvariantFunctor>(index, x);
}

template <typename T>
struct ToBoostAnyFunctor
{
	typedef boost::any return_type;
	
	template <int I>
	static return_type call(QVariant const& variant)
	{
		typedef typename friendly_fusion::utils::DecayedTypeOfAtIndex<T, I>::type value_type;
		
		return boost::any(variant.value<value_type>());
	}
};

template <typename T>
boost::any to_boost_any(QVariant const& x, int index)
{
	return apply_functor_to_member<T, ToBoostAnyFunctor>(index, x);
}
