#pragma once

#include "index_list.hpp"
#include "friendly_fusion.hpp"

#include <boost/any.hpp>

#include <algorithm>
#include <string>
#include <functional>
#include <type_traits>

namespace boost { namespace fusion { template <typename Sequence1, typename Sequence2> struct joint_view; } }

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

template <typename T>
struct is_joint_view : public std::false_type {};

template <typename T, typename U>
struct is_joint_view<boost::fusion::joint_view<T, U>> : public std::true_type{};

template <typename T>
struct joint_view_first_t {};

template <typename T, typename U>
struct joint_view_first_t<boost::fusion::joint_view<T, U>> {
	typedef T type;
};

template <typename T>
struct joint_view_second_t {};

template <typename T, typename U>
struct joint_view_second_t<boost::fusion::joint_view<T, U>> {
	typedef U type;
};

template <typename U, template<typename T> class F, typename... Args>
typename std::enable_if<is_joint_view<U>::value, typename std::common_type<typename F<typename joint_view_first_t<U>::type>::return_type , typename F<typename joint_view_second_t<U>::type>::return_type>::type>::type
apply_functor_to_member(int index, Args... args)
{
	typedef typename joint_view_first_t<U>::type S1;
	typedef typename joint_view_second_t<U>::type S2;
	constexpr int size_s1 = friendly_fusion::result_of::size<S1>::type::value;
	
	if(index < size_s1) {
		return apply_functor_to_member_impl<S1, F>(typename IndicesOf<S1>::type{}, index, std::forward<Args...>(args)...);
	} else {
		return apply_functor_to_member_impl<S2, F>(typename IndicesOf<S2>::type{}, index - size_s1, std::forward<Args...>(args)...);
	}
}

template <typename U, template<typename T> class F, typename... Args>
typename std::enable_if<!is_joint_view<U>::value, typename F<U>::return_type>::type
apply_functor_to_member(int index, Args... args)
{
	return apply_functor_to_member_impl<U, F>(typename IndicesOf<U>::type{}, index, std::forward<Args...>(args)...);
}

template <template<typename T> class F, typename Arg1, typename... Args>
typename std::enable_if<is_joint_view<Arg1>::value, typename std::common_type<typename F<typename joint_view_first_t<Arg1>::type>::return_type , typename F<typename joint_view_second_t<Arg1>::type>::return_type>::type>::type
apply_functor_to_member(int index, Arg1& arg1, Args... args)
{
	typedef typename joint_view_first_t<Arg1>::type S1;
	constexpr int size_s1 = friendly_fusion::result_of::size<S1>::type::value;
	
	if(index < size_s1) {
		return apply_functor_to_member_impl<F>(typename IndicesOf<Arg1>::type{}, index, arg1, std::forward<Args...>(args)...);
	} else {
		return apply_functor_to_member_impl<F>(typename IndicesOf<Arg1>::type{}, index - size_s1, arg1, std::forward<Args...>(args)...);
	}
}

template <template<typename T> class F, typename Arg1, typename... Args>
typename std::enable_if<!is_joint_view<Arg1>::value, typename F<Arg1>::return_type>::type
apply_functor_to_member(int index, Arg1& arg1, Args... args)
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
auto is_const(int index)
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
auto get_nth(T x, int index)
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
auto set_nth(T&& x, int index, boost::any const& value)
{
	apply_functor_to_member<set_nth_functor>(index, x, value);
}

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

template <typename T>
auto get_nth_name(int index)
{
	return apply_functor_to_member<T, get_nth_name_functor>(index);
}
