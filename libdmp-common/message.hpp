#pragma once

#include "dmp-library.hpp"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

#include <cstdint>

namespace message {

enum class Type : uint32_t {
	NoMessage,
	Ping,
	Pong,
	NameRequest,
	NameResponse,
	SearchRequest,
	SearchResponse,
	Bye,
	ByeAck,
	Radio,
	RadioAck,
	AddRadio,
	AddRadioResponse,
	ListenConnectionRequest,
	Radios,
};

struct Ping {

	Type type;
	std::string ping;
	std::string payload;

	struct RandDevice
	{
		RandDevice()
		{
			srand(time(nullptr));
		}

		int operator()() const
		{
			return rand();
		}
	};

	std::string random_string()
	{
		static const std::string alphanum =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		static const RandDevice rand;

		std::string ret = "";
		for (int i = 0; i < 12; ++i) {
			ret += alphanum[rand() % (alphanum.size() - 1)];
		}

		return ret;
	}

	Ping()
	: type(Type::Ping)
	, ping("Ping: ")
	, payload(random_string())
	{}
};

struct Pong
{
	Type type;
	std::string pong;
	std::string payload;

	Pong()
	: type(Type::Pong)
	, pong("Pong: ")
	, payload("")
	{}

	Pong(message::Ping p)
	: type(Type::Pong)
	, pong("Pong: ")
	, payload(p.payload)
	{}
};

struct NameRequest
{
	Type type;

	NameRequest()
	: type(Type::NameRequest)
	{}

};

struct NameResponse
{
	Type type;
	std::string name;

	NameResponse()
	: type(Type::NameResponse)
	, name("")
	{}

	NameResponse(std::string name)
	: type(Type::NameResponse)
	, name(name)
	{}
};

struct SearchRequest
{
	Type type;
	std::string query;

	SearchRequest()
	: type(Type::SearchRequest)
	, query()
	{}

	SearchRequest(std::string query)
	: type(Type::SearchRequest)
	, query(query)
	{}
};

struct SearchResponse
{
	Type type;
	std::string origin;
	std::string query;
	std::vector<dmp_library::LibraryEntry> results;

	SearchResponse()
	: type(Type::SearchResponse)
	, origin()
	, query()
	, results()
	{}

	SearchResponse(std::string query, std::vector<dmp_library::LibraryEntry> results, std::string origin)
	: type(Type::SearchResponse)
	, origin(origin)
	, query(query)
	, results(results)
	{}
};

struct Bye
{
	Type type;

	Bye()
	: type(Type::Bye)
	{}
};

struct ByeAck
{
	Type type;

	ByeAck()
	: type(Type::ByeAck)
	{}
};

struct Radio {
	Type type;

	Radio()
	: type(Type::Radio)
	{}
};

struct AddRadio {
	Type type;
	std::string name;

	AddRadio()
	: type(Type::AddRadio)
	, name()
	{}

	AddRadio(std::string name)
	: type(Type::AddRadio)
	, name(name)
	{}
};

struct AddRadioResponse {
	Type type;
	bool succes;
	std::string reason;

	AddRadioResponse()
	: type(Type::AddRadioResponse)
	, succes()
	, reason()
	{}

	AddRadioResponse(bool succes, std::string reason="")
	: type(Type::AddRadioResponse)
	, succes(succes)
	, reason()
	{}
};

struct ListenConnectionRequest {
	Type type;
	uint16_t port;

	ListenConnectionRequest()
	: type(Type::ListenConnectionRequest)
	, port()
	{}

	ListenConnectionRequest(uint16_t port)
	: type(Type::ListenConnectionRequest)
	, port(port)
	{}
};
/*
struct ListenConnectionRequestAck {
	Type type;

	ListenConnectionRequestAck()
	: type(Type::ListenConnectionRequestAck)
	{}
};
*/
struct Radios {
	Type type;
	std::map<std::string, std::vector<dmp_library::LibraryEntry>> radios;

	Radios()
	: type(Type::Radios)
	, radios()
	{}

	Radios(std::map<std::string, std::vector<dmp_library::LibraryEntry>> radios)
	: type(Type::Radios)
	, radios(radios)
	{}
};
/*
struct RadiosAck {
	Type type;

	RadiosAck()
	: type(Type::RadiosAck)
	{}
};
*/
}

BOOST_FUSION_ADAPT_STRUCT(
	message::Ping,
	(message::Type, type)
	(std::string, ping)
	(std::string, payload)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::Pong,
	(message::Type, type)
	(std::string, pong)
	(std::string, payload)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::NameRequest,
	(message::Type, type)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::NameResponse,
	(message::Type, type)
	(std::string, name)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::SearchRequest,
	(message::Type, type)
	(std::string, query)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::SearchResponse,
	(message::Type, type)
	(std::string, origin)
	(std::string, query)
	(std::vector<dmp_library::LibraryEntry>, results)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::Bye,
	(message::Type, type)
)


BOOST_FUSION_ADAPT_STRUCT(
	message::ByeAck,
	(message::Type, type)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::AddRadio,
	(message::Type, type)
	(std::string, name)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::AddRadioResponse,
	(message::Type, type)
	(bool, succes)
	(std::string, reason)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::ListenConnectionRequest,
	(message::Type, type)
	(uint16_t, port)
)
/*
BOOST_FUSION_ADAPT_STRUCT(
	message::ListenConnectionRequestAck,
	(message::Type, type)
)
*/
//To not confuse the macro expansion about macro parameters.
typedef std::map<std::string, std::vector<dmp_library::LibraryEntry>> radios_type;

BOOST_FUSION_ADAPT_STRUCT(
	message::Radios,
	(message::Type, type)
	(radios_type, radios)
)
/*
BOOST_FUSION_ADAPT_STRUCT(
	message::RadiosAck,
	(message::Type, type)
)
*/
