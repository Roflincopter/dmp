#pragma once

#include <boost/fusion/adapted/struct/detail/extension.hpp>
#include <boost/fusion/iterator/advance.hpp>
#include <boost/fusion/iterator/deref.hpp>
#include <boost/fusion/sequence/intrinsic/begin.hpp>
#include <boost/fusion/sequence/intrinsic/end.hpp>
#include <boost/fusion/sequence/intrinsic/size.hpp>
#include <boost/fusion/support/is_iterator.hpp>
#include <boost/fusion/support/is_sequence.hpp>
#include <boost/fusion/support/is_view.hpp>

#include <type_traits>

namespace friendly_fusion {

namespace traits {

template <typename T, bool NoCheck = true>
struct is_sequence : public boost::fusion::traits::is_sequence<T>
{
	static_assert(NoCheck || boost::fusion::traits::is_sequence<T>::value, "Template parameter: T, is not a fusion adapted struct.");
};

template <typename T, bool NoCheck = true>
struct is_fusion_iterator : public boost::fusion::is_fusion_iterator<T>
{
	static_assert(NoCheck || boost::fusion::is_fusion_iterator<T>::value, "Template parameter: T. Is not a fusion iterator.");
};

template <typename T, bool NoCheck = true>
struct is_view : public boost::fusion::traits::is_view<T> {
	static_assert(NoCheck || boost::fusion::traits::is_view<T>::value, "Template parameter: T, is not a fusion view");
};

template <typename T, bool NoCheck = true>
struct is_sequence_or_view : public boost::mpl::or_<typename boost::fusion::traits::is_sequence<T>, typename boost::fusion::traits::is_view<T>> {
	static_assert(NoCheck || (boost::fusion::traits::is_sequence<T>::value || boost::fusion::traits::is_view<T>::value), "Template parameter: T, is not a fusion sequence or a view");
};

}

template <typename T>
typename std::enable_if<traits::is_fusion_iterator<T, false>::value, typename boost::fusion::result_of::deref<T>::type>::type
deref(T const& x)
{
	return boost::fusion::deref(x);
}

template <int N, typename T>
typename std::enable_if<traits::is_fusion_iterator<T, false>::value, typename boost::fusion::result_of::advance_c<T, N>::type>::type
advance_c(T const& x)
{
	return boost::fusion::advance_c<N>(x);
}

template <typename T>
typename std::enable_if<traits::is_sequence_or_view<T, false>::value, typename boost::fusion::result_of::begin<T>::type>::type const
begin(T& x)
{
	return boost::fusion::begin(x);
}

template <typename T>
typename std::enable_if<traits::is_sequence_or_view<T, false>::value, typename boost::fusion::result_of::begin<T const>::type>::type const
begin(T const& x)
{
	return boost::fusion::begin(x);
}

template <typename T>
typename std::enable_if<traits::is_sequence_or_view<T, false>::value, typename boost::fusion::result_of::end<T>::type>::type const
end(T& x)
{
	return boost::fusion::end(x);
}

template <typename T>
typename std::enable_if<traits::is_sequence_or_view<T, false>::value, typename boost::fusion::result_of::end<T const>::type>::type const
end(T const& x)
{
	return boost::fusion::end(x);
}

namespace result_of {

template <typename T>
struct size : private traits::is_sequence_or_view<T, false>, public boost::fusion::result_of::size<T>
{
	using typename boost::fusion::result_of::size<T>::type;
};

template <typename T>
struct begin : private traits::is_sequence_or_view<T, false>, public boost::fusion::result_of::begin<T>
{
	using typename boost::fusion::result_of::begin<T>::type;
};

template <typename T>
struct end : private traits::is_sequence_or_view<T, false>, public boost::fusion::result_of::end<T>
{
	using typename boost::fusion::result_of::end<T>::type;
};

template <typename T, int N>
struct advance_c : private traits::is_fusion_iterator<T, false>, public boost::fusion::result_of::advance_c<T, N>
{
	using typename boost::fusion::result_of::advance_c<T, N>::type;
};

template <typename T>
struct deref : private traits::is_fusion_iterator<T, false>, public boost::fusion::result_of::deref<T>
{
	using typename boost::fusion::result_of::deref<T>::type;
};

}

namespace extension {

template<typename T, int index>
struct struct_member_name : private traits::is_sequence<T, false>, public boost::fusion::extension::struct_member_name<T, index>
{};

}

namespace utils {

template<typename T, int I>
class DecayedTypeOfAtIndex {
	typedef friendly_fusion::result_of::begin<T> begin;
	typedef friendly_fusion::result_of::advance_c<typename begin::type, I> adv_it;
	typedef friendly_fusion::result_of::deref<typename adv_it::type> deref;
	typedef std::decay<typename deref::type> decayed;

public:
	typedef typename decayed::type type;
};

template<typename T, int I>
class RefTypeOfAtIndex {
	typedef friendly_fusion::result_of::begin<T> begin;
	typedef friendly_fusion::result_of::advance_c<typename begin::type, I> adv_it;
	typedef friendly_fusion::result_of::deref<typename adv_it::type> deref;

public:
	typedef typename deref::type type;
};

template<typename T, int I>
class UnrefTypeOfAtIndex {
	typedef friendly_fusion::result_of::begin<T> begin;
	typedef friendly_fusion::result_of::advance_c<typename begin::type, I> adv_it;
	typedef friendly_fusion::result_of::deref<typename adv_it::type> deref;
	typedef std::remove_reference<typename deref::type> unrefed;

public:
	typedef typename unrefed::type type;
};

}

}