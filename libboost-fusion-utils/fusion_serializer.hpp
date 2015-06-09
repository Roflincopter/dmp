#pragma once

#include "friendly_fusion.hpp"
#include "fusion_static_dispatch.hpp"
#include "index_list.hpp"

#include <tuple>
#include <type_traits>

namespace message {

struct Serialize
{
	template <typename Archive, typename T>
	static typename std::enable_if<friendly_fusion::traits::is_sequence<T>::value, void>::type
	serialize(Archive& ar, T& x)
	{
		Serialize::serialize(ar, friendly_fusion::begin(x), friendly_fusion::end(x));
	}

	template <typename Archive, typename T>
	static typename std::enable_if<!friendly_fusion::traits::is_sequence<T>::value, void>::type
	serialize(Archive& ar, T& x)
	{
		ar & x;
	}

	template <typename Archive, typename I, typename E>
	static typename std::enable_if<!std::is_same<I,E>::value, void>::type
	serialize(Archive& ar, I const& it, E const& end)
	{
		Serialize::serialize(ar, friendly_fusion::deref(it));
		Serialize::serialize(ar, friendly_fusion::advance_c<1>(it), end);
	}

	template <typename Archive, typename I, typename E>
	static typename std::enable_if<std::is_same<I,E>::value, void>::type
	serialize(Archive&, I, E)
	{
		return;
	}
};

template <typename Archive, typename T>
typename std::enable_if<friendly_fusion::traits::is_sequence<T>::value, void>::type
serialize(Archive& ar, T& x)
{
	Serialize::serialize(ar, friendly_fusion::begin(x), friendly_fusion::end(x));
}

template <typename T, typename Archive>
typename std::enable_if<friendly_fusion::traits::is_sequence<T>::value && std::is_default_constructible<T>::value, T>::type
serialize(Archive& ar)
{
	T x;
	Serialize::serialize(ar, friendly_fusion::begin(x), friendly_fusion::end(x));
	return x;
}

//Base case of unpack for fusion adapted member
template <typename T, int Index, typename Archive>
typename std::enable_if<
	friendly_fusion::traits::is_sequence<typename friendly_fusion::utils::DecayedTypeOfAtIndex<T, Index>::type>::value,
	typename friendly_fusion::utils::DecayedTypeOfAtIndex<T,Index>::type
>::type
serialize(Archive& ar);

//Base case of unpack for non-fusion adapted member
template <typename T, int Index, typename Archive>
typename std::enable_if<
	!friendly_fusion::traits::is_sequence<typename friendly_fusion::utils::DecayedTypeOfAtIndex<T, Index>::type>::value,
	typename friendly_fusion::utils::DecayedTypeOfAtIndex<T,Index>::type
>::type
serialize(Archive& ar);

//Unpacker.
template <typename T, typename Archive, int... Indices>
typename std::enable_if<friendly_fusion::traits::is_sequence<T>::value, T>::type
serialize(Archive& ar, indices<Indices...>);

//Setup for unpack.
template <typename T, typename Archive>
typename std::enable_if<friendly_fusion::traits::is_sequence<T>::value && !std::is_default_constructible<T>::value, T>::type
serialize(Archive& ar)
{
	return serialize<T>(ar, typename IndicesOf<T>::type{});
}

//Unpacker impl.
template <typename T, typename Archive, int... Indices>
typename std::enable_if<friendly_fusion::traits::is_sequence<T>::value, T>::type
serialize(Archive& ar, indices<Indices...>)
{
	//force the right order of evalutaion.
	std::tuple<typename friendly_fusion::utils::DecayedTypeOfAtIndex<T, Indices>::type...> tuple{serialize<T, Indices>(ar)...};
	return T(std::get<Indices>(tuple)...);
}

//Base case for fusion impl
template <typename T, int Index, typename Archive>
typename std::enable_if<
	friendly_fusion::traits::is_sequence<typename friendly_fusion::utils::DecayedTypeOfAtIndex<T, Index>::type>::value,
	typename friendly_fusion::utils::DecayedTypeOfAtIndex<T,Index>::type
>::type
serialize(Archive& ar)
{
	return serialize<typename friendly_fusion::utils::DecayedTypeOfAtIndex<T, Index>::type>(ar);
}

//Base case for non-fusion impl
template <typename T, int Index, typename Archive>
typename std::enable_if<
	!friendly_fusion::traits::is_sequence<typename friendly_fusion::utils::DecayedTypeOfAtIndex<T, Index>::type>::value,
	typename friendly_fusion::utils::DecayedTypeOfAtIndex<T,Index>::type
>::type
serialize(Archive& ar)
{
	typename friendly_fusion::utils::DecayedTypeOfAtIndex<T, Index>::type t;
	ar & t;
	return t;
}

}
