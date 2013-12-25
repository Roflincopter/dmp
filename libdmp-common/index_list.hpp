#pragma once

template<int... Indices>
struct indices {
	typedef indices<Indices..., sizeof...(Indices)> next;
};

template<int N>
struct build_indices {
	typedef typename build_indices<N - 1>::type::next type;
};

template<>
struct build_indices<0> {
	typedef indices<> type;
};
