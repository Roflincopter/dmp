
#include "fusion_serializer.hpp"
#include "archive.hpp"
#include "fusion_lexicographical_compare.hpp"

#include <boost/fusion/adapted/struct.hpp>
#include <sstream>

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

int main() {
	std::vector<bool> results;
	{
		message::Struct i1{"string", 10};
	
		std::stringstream ss;
		OArchive oar(ss);
		message::serialize(oar, i1);
		
		IArchive iar(ss);
		message::Struct o1 = message::serialize<message::Struct>(iar);
		
		results.push_back(i1 == o1);
	}
	
	{
		message::StructV i2{{{get_random_vector(), "string1", 10}, {get_random_vector(), "string2 ", 20}}};
		std::stringstream ss;
		OArchive oar(ss);
		message::serialize(oar, i2);
		
		IArchive iar(ss);
		message::StructV o2 = message::serialize<message::StructV>(iar);
		
		results.push_back(i2 == o2);
	}
	
	{
		message::StructM i3{{{"key1", {get_random_vector(), "string1", 10}}, {"key2", {get_random_vector(), "string2", 20}}}};
		std::stringstream ss;
		OArchive oar(ss);
		message::serialize(oar, i3);
		
		IArchive iar(ss);
		message::StructM o3 = message::serialize<message::StructM>(iar);
		
		results.push_back(i3 == o3);
	}
	
	{
		message::StructVM i4{
			{{get_random_vector(), "string1", 10}, {get_random_vector(), "string2", 20}},
			{{"key1", {get_random_vector(), "string1", 10}}, {"key2", {get_random_vector(), "string2", 20}}}
		};
		std::stringstream ss;
		OArchive oar(ss);
		message::serialize(oar, i4);
		
		IArchive iar(ss);
		message::StructVM o4 = message::serialize<message::StructVM>(iar);
		
		results.push_back(i4 == o4);
	}
	
	{
		message::StructMV i5{
			{{"key1", {get_random_vector(), "string1", 10}}, {"key2", {get_random_vector(), "string2", 20}}},
			{{get_random_vector(), "string1", 10}, {get_random_vector(), "string2 ", 20}}
		};
		std::stringstream ss;
		OArchive oar(ss);
		message::serialize(oar, i5);
		
		IArchive iar(ss);
		message::StructMV o5 = message::serialize<message::StructMV>(iar);
		
		results.push_back(i5 == o5);
	}
	
	{
		message::StructMoV i6 {
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
		
		std::stringstream ss;
		OArchive oar(ss);
		message::serialize(oar, i6);
		
		IArchive iar(ss);
		message::StructMoV o6 = message::serialize<message::StructMoV>(iar);
		
		results.push_back(i6 == o6);
	}

	if(std::all_of(results.begin(), results.end(), &id<bool>)) {
		return 0;
	} else {
		return 1;
	}
}
