#pragma once

#ifdef __MINGW64__
#define __USE_ISOC99
#endif

#include <boost/numeric/interval.hpp>

#ifdef __MINGW64__
#undef __USE_ISOC99
#endif

#include <limits>
#include <set>
#include <iostream>

class NumberInterval
{
	boost::numeric::interval<int> value;

public:
	NumberInterval(int lower, int upper) : value(lower, upper) {}

	bool operator < (const NumberInterval& s) const {
		return (value.lower() < s.value.lower()) && (value.upper() < s.value.lower());
	}

	int left() const {
		return value.lower();
	}

	int right() const {
		return value.upper();
	}
};

class NumberPool {
	typedef std::set<NumberInterval> NumberIntervals_t;
	NumberIntervals_t free;

public:
	NumberPool(int lower, int upper);
	int allocate_number();
	void free_number(int id);

};
