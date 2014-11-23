#pragma once

#include "index_list.hpp"
#include "friendly_fusion.hpp"

#include <boost/any.hpp>

#include <functional>
#include <iostream>

template <typename T>
struct IndicesOf 
{
	typedef typename build_indices<friendly_fusion::result_of::size<T>::type::value>::type type;
};

template <typename U, template<typename T> class F, int... Indices, typename... Args>
typename F<U>::return_type apply_functor_to_member_impl(indices<Indices...>, int index, Args... args)
{
	typedef std::function<typename F<U>::return_type(Args...)> f_type;
	static f_type array[] = {
		F<U>::template call<Indices>
		...
	};
	
	return array[index](std::forward<Args...>(args)...);
}

template <template<typename T> class F, int... Indices, typename Arg1, typename... Args>
typename F<Arg1>::return_type apply_functor_to_member_impl(indices<Indices...>, int index, Arg1& arg1, Args... args)
{
	typedef std::function<typename F<Arg1>::return_type(Arg1&, Args...)> f_type;
	static f_type array[] = {
		F<Arg1>::template call<Indices>
		...
	};
	
	return array[index](arg1, std::forward<Args...>(args)...);
}

template <typename U, template<typename T> class F, typename... Args>
typename F<U>::return_type apply_functor_to_member(int index, Args... args)
{
	return apply_functor_to_member_impl<U, F>(typename IndicesOf<U>::type{}, index, std::forward<Args...>(args)...);
}

template <template<typename T> class F, typename Arg1, typename... Args>
typename F<Arg1>::return_type apply_functor_to_member(int index, Arg1& arg1, Args... args)
{
	return apply_functor_to_member_impl<F>(typename IndicesOf<Arg1>::type{}, index, arg1, std::forward<Args...>(args)...);
}

template <typename T>
struct is_const_functor
{
	typedef bool return_type;
	
	template <int I>
	static return_type call()
	{
		typedef typename friendly_fusion::utils::UnrefTypeOfAtIndex<T, I>::type unref_type;
		
		return std::is_const<unref_type>::value;
	}
};

template <typename T>
bool is_const(int index)
{
	return apply_functor_to_member<T, is_const_functor>(index);
}

template <typename T>
struct get_nth_functor
{
	typedef boost::any return_type;
	
	template <int I>
	static return_type call(T& seq)
	{
		return boost::any(friendly_fusion::deref(friendly_fusion::advance_c<I>(friendly_fusion::begin(seq))));
	}
};

template <typename T>
boost::any get_nth(T x, int index)
{
	return apply_functor_to_member<get_nth_functor>(index, x);
}

template <bool b, typename T, typename V>
typename std::enable_if<b, void>::type assign(T&, V)
{
	//this function will never be called, but has to be defined.
	//We need to generate a assign statement for all members at compile time even when they are	const.
	return;
}

template <bool b, typename T, typename V>
typename std::enable_if<!b, void>::type assign(T& lh, V rh)
{
	lh = rh;
}

template <typename T>
struct set_nth_functor
{
	typedef void return_type;
	
	template <int I>
	static return_type call(T& t, boost::any const& value)
	{
		typedef typename friendly_fusion::utils::DecayedTypeOfAtIndex<T, I>::type value_type;
		typedef typename friendly_fusion::utils::UnrefTypeOfAtIndex<T, I>::type unref_type;
		
		assign<std::is_const<unref_type>::value>(friendly_fusion::deref(friendly_fusion::advance_c<I>(friendly_fusion::begin(t))), boost::any_cast<value_type>(value));
	}
};

template <typename T>
void set_nth(T&& x, int index, boost::any const& value)
{
	apply_functor_to_member<set_nth_functor>(index, x, value);
}

template <typename T>
std::string get_nth_name(int index);

template <typename T>
struct get_nth_name_functor 
{
	typedef std::string return_type;
	
	template <int I>
	static return_type call()
	{
		return std::string(friendly_fusion::extension::struct_member_name<T, I>::call());
	}
};

template <typename T, typename U>
struct get_nth_name_functor<boost::fusion::joint_view<T,U>>
{
	typedef std::string return_type;
	
	template <int I>
	static return_type call()
	{
		constexpr int size_of_T = friendly_fusion::result_of::size<T>::type::value;
		if(I < size_of_T){
			return get_nth_name<T>(I);
		} else {
			return get_nth_name<U>(I - size_of_T);
		}
	}
};

template <typename T>
std::string get_nth_name(int index)
{
	return apply_functor_to_member<T, get_nth_name_functor>(index);
}
