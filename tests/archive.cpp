
#include "fusion_serializer.hpp"
#include "archive.hpp"
#include "fusion_lexicographical_compare.hpp"

#include <boost/fusion/adapted/struct.hpp>
#include <sstream>
#include <random>

struct Entry {
	std::vector<uint8_t> vec;
	std::string str;
	int i;
	
	friend bool operator==(Entry lh, Entry rh) {
		return 
			lh.vec == rh.vec &&
			lh.str == rh.str &&
			lh.i == rh.i;
	}
	
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int)
	{
		ar & vec & str & i;
	}
};

namespace message {
struct Struct {
	std::string str;
	int i;
	
	friend bool operator==(Struct lh, Struct rh) {
		return 
			lh.str == rh.str && 
			lh.i == rh.i;
	}
};

struct StructV {
	std::vector<Entry> v;
	
	friend bool operator==(StructV lh, StructV rh) {
		return lh.v == rh.v;
	}
};


struct StructM {
	std::map<std::string, Entry> m;
	
	friend bool operator==(StructM lh, StructM rh) {
		return lh.m == rh.m;
	}
};

struct StructVM {
	std::vector<Entry> v;
	std::map<std::string, Entry> m;
	
	friend bool operator==(StructVM lh, StructVM rh) {
		return lh.v == rh.v && lh.m == rh.m;
	}
};

struct StructMV {
	std::map<std::string, Entry> m;
	std::vector<Entry> v;

	friend bool operator==(StructMV lh, StructMV rh) {
		return lh.v == rh.v && lh.m == rh.m;
	}
};

struct StructMoV {
	std::map<std::string, std::vector<Entry>> m;
	
	friend bool operator==(StructMoV lh, StructMoV rh) {
		return lh.m == rh.m;
	}
};

struct StructMM {
	std::multimap<std::string, Entry> m;
	
	friend bool operator==(StructMM lh, StructMM rh) {
		return lh.m == rh.m;
	}
};

struct StructMMV {
	std::multimap<std::string, Entry> m;
	std::vector<Entry> v;

	friend bool operator==(StructMMV lh, StructMMV rh) {
		return lh.v == rh.v && lh.m == rh.m;
	}
};


struct StructVMM {
	std::vector<Entry> v;
	std::multimap<std::string, Entry> m;

	friend bool operator==(StructVMM lh, StructVMM rh) {
		return lh.v == rh.v && lh.m == rh.m;
	}
};

struct StructMMoV {
	std::multimap<std::string, std::vector<Entry>> m;
	
	friend bool operator==(StructMMoV lh, StructMMoV rh) {
		return lh.m == rh.m;
	}
};


}

BOOST_FUSION_ADAPT_STRUCT(
	message::Struct,
	(std::string, str)
	(int, i)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::StructV,
	(std::vector<Entry>, v)
)

typedef std::map<std::string, Entry> MapT;

BOOST_FUSION_ADAPT_STRUCT(
	message::StructM,
	(MapT, m)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::StructVM,
	(std::vector<Entry>, v)
	(MapT, m)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::StructMV,
	(MapT, m)
	(std::vector<Entry>, v)
)

typedef std::map<std::string, std::vector<Entry>> MoVMapT;

BOOST_FUSION_ADAPT_STRUCT(
	message::StructMoV,
	(MoVMapT, m)
)

typedef std::multimap<std::string,Entry> MMapT;

BOOST_FUSION_ADAPT_STRUCT(
	message::StructMM,
	(MMapT, m)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::StructVMM,
	(std::vector<Entry>, v)
	(MMapT, m)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::StructMMV,
	(MMapT, m)
	(std::vector<Entry>, v)
)

typedef std::multimap<std::string, std::vector<Entry>> MMoVMapT;

BOOST_FUSION_ADAPT_STRUCT(
	message::StructMMoV,
	(MMoVMapT, m)
)

template<typename T>
T&& id(T&& x) {return std::move(x);}

std::vector<uint8_t> get_random_vector() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> dis(0, 255);

	auto v = std::vector<uint8_t>(20, 0);
	std::transform(v.begin(), v.end(), v.begin(), [](uint8_t const&){
		return dis(gen);
	});
	return v;
}

template <typename T>
T s_and_d(T t) {
	std::stringstream ss;
	OArchive oar(ss);
	message::serialize(oar, t);
	
	IArchive iar(ss);
	return message::serialize<T>(iar);
}

bool test1() {
	message::Struct i{"string", 10};
	message::Struct o = s_and_d(i);
	
	return i == o;
}

bool test2() {
	message::StructV i{{{get_random_vector(), "string1", 10}, {get_random_vector(), "string2 ", 20}}};
	auto o = s_and_d(i);
	
	return i == o;
}

bool test3() {
	message::StructM i{{{"key1", {get_random_vector(), "string1", 10}}, {"key2", {get_random_vector(), "string2", 20}}}};
	auto o = s_and_d(i);
	
	return i == o;
}

bool test4() {
	message::StructVM i{
		{{get_random_vector(), "string1", 10}, {get_random_vector(), "string2", 20}},
		{{"key1", {get_random_vector(), "string1", 10}}, {"key2", {get_random_vector(), "string2", 20}}}
	};
	auto o = s_and_d(i);
	
	return i == o;
}

bool test5() {
	message::StructMV i{
		{{"key1", {get_random_vector(), "string1", 10}}, {"key2", {get_random_vector(), "string2", 20}}},
		{{get_random_vector(), "string1", 10}, {get_random_vector(), "string2 ", 20}}
	};
	auto o = s_and_d(i);
	
	return i == o;
}

bool test6() {
	message::StructMoV i {
		{
			{
				"key1",
				{{get_random_vector(), "string1", 0}, {get_random_vector(), "string2", 20}}
			},
			{
				"key2",
				{{get_random_vector(), "string1", 10}, {get_random_vector(), "string2", 20}}
			}
		}
	};
	auto o = s_and_d(i);
	
	return i == o;
}

bool test7() {
	message::StructMM i {
		{
			{
				"key1",
				{get_random_vector(), "string1", 0}
			},
			{
				"key1",
				{get_random_vector(), "string2", 30}
			},
			{
				"key2",
				{get_random_vector(), "string3", 40}
			}
		}
	};
	auto o = s_and_d(i);
	
	return i == o;
}

bool test8() {
	message::StructVMM i{
		{{get_random_vector(), "string1", 10}, {get_random_vector(), "string2", 20}},
		{{"key1", {get_random_vector(), "string1", 10}}, {"key1", {get_random_vector(), "string3", 50}}, {"key2", {get_random_vector(), "string2", 20}}}
	};
	auto o = s_and_d(i);
	
	return i == o;
}

bool test9() {
	message::StructMMV i{
		{{"key1", {get_random_vector(), "string1", 10}}, {"key1", {get_random_vector(), "string3", 50}}, {"key2", {get_random_vector(), "string2", 20}}},
		{{get_random_vector(), "string1", 10}, {get_random_vector(), "string2", 20}}
	};
	auto o = s_and_d(i);
	
	return i == o;
}

bool test10() {
	message::StructMoV i {
		{
			{
				"key1",
				{{get_random_vector(), "string1", 0}, {get_random_vector(), "string2", 20}}
			},
			{
				"key1",
				{{get_random_vector(), "string3", 40}, {get_random_vector(), "string2", 20}}
			},
			{
				"key2",
				{{get_random_vector(), "string1", 10}, {get_random_vector(), "string2", 20}}
			}
		}
	};
	auto o = s_and_d(i);
	
	return i == o;
}


int main() {
	std::vector<bool> results{test1(), test2(), test3(), test4(), test5(), test6(), test7(), test8(), test9(), test10()};

	if(std::all_of(results.begin(), results.end(), &id<bool>)) {
		return 0;
	} else {
		return 1;
	}
}
