#pragma once

#include <boost/fusion/adapted/struct/adapt_struct.hpp>

struct RadioState
{
	bool playing;

	RadioState();
	RadioState(bool playing);

	template <typename Archive>
	void serialize(Archive& ar, unsigned int)
	{
		ar & playing;
	}
};

BOOST_FUSION_ADAPT_STRUCT(
	RadioState,
	(bool, playing)
)

