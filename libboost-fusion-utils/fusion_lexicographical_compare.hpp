#pragma once

#include "friendly_fusion.hpp"

namespace ff = friendly_fusion;

struct LexicographicalCompare
{
	template <typename I1, typename E, typename I2>
	static typename std::enable_if<!std::is_same<I1,E>::value, bool>::type
	less_than(I1 const& it1, E const& end, I2 const& it2)
	{
		if(ff::deref(it1) < ff::deref(it2)) {
			return true;
		}
		if(ff::deref(it1) == ff::deref(it2)) {
			return LexicographicalCompare::less_than(ff::advance_c<1>(it1), end, ff::advance_c<1>(it2));
		}
		return false;
	}

	template <typename I1, typename E, typename I2>
	static typename std::enable_if<std::is_same<I1,E>::value, bool>::type
	less_than(I1 const&, E const&, I2 const&)
	{
		return false;
	}
};


template <typename T>
bool lexicographical_compare(T const& lh, T const& rh)
{
	return LexicographicalCompare::less_than(ff::begin(lh), ff::end(lh), ff::begin(rh));
}
