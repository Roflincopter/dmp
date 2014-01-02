#pragma once

#include "dmp-library.hpp"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

#include <cstdint>

namespace message {

typedef uint32_t Type_t;
typedef std::vector<std::tuple<std::string, std::string, dmp_library::LibraryEntry>> Playlist;

enum class Type : Type_t {
	NoMessage,
	Ping,
	Pong,
	NameRequest,
	NameResponse,
	SearchRequest,
	SearchResponse,
	Bye,
	ByeAck,
	AddRadio,
	AddRadioResponse,
	ListenConnectionRequest,
	Radios,
	Queue,
	PlaylistUpdate,
	LAST
};

struct NoMessage {
	Type type;

	NoMessage()
	{
		throw std::runtime_error("No NoMessage struct should be made ever.");
	}
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
	std::string radio_name;
	bool succes;
	std::string reason;

	AddRadioResponse()
	: type(Type::AddRadioResponse)
	, radio_name()
	, succes()
	, reason()
	{}

	AddRadioResponse(std::string radio_name, bool succes, std::string reason="")
	: type(Type::AddRadioResponse)
	, radio_name(radio_name)
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
	std::map<std::string, Playlist> radios;

	Radios()
	: type(Type::Radios)
	, radios()
	{}

	Radios(std::map<std::string, Playlist> radios)
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

struct Queue {
	Type type;
	std::string radio;
	std::string queuer;
	std::string owner;
	dmp_library::LibraryEntry entry;

	Queue()
	: type(Type::Queue)
	, radio()
	, queuer()
	, owner()
	, entry()
	{}

	Queue(std::string radio, std::string queuer, std::string owner, dmp_library::LibraryEntry entry)
	: type(Type::Queue)
	, radio(radio)
	, queuer(queuer)
	, owner(owner)
	, entry(entry)
	{}
};

struct PlaylistUpdate {
	struct Action
	{
		enum class Type
		{
			NoAction,
			Append,
			Insert,
			Move,
			Reset,
		};

		Type type;
		uint32_t from;
		uint32_t to;

		Action()
		: type(Type::NoAction)
		, from()
		, to()
		{}

		Action(Type type, uint32_t from, uint32_t to)
		: type(type)
		, from(from)
		, to(to)
		{}
	};

	Type type;
	Action action;
	std::string radio_name;
	Playlist playlist;

	PlaylistUpdate()
	: type(Type::PlaylistUpdate)
	, radio_name()
	, playlist()
	{}

	PlaylistUpdate(Action action, std::string radio_name, Playlist playlist)
	: type(Type::PlaylistUpdate)
	, action(action)
	, radio_name(radio_name)
	, playlist(playlist)
	{}
};

template <Type t>
struct type_to_message
{
	typedef NoMessage type;
};

#define TYPE_TO_MESSAGE_STRUCT(X) \
template<>\
struct type_to_message<Type::X>\
{\
	typedef X type;\
};

TYPE_TO_MESSAGE_STRUCT(Ping)
TYPE_TO_MESSAGE_STRUCT(Pong)
TYPE_TO_MESSAGE_STRUCT(NameRequest)
TYPE_TO_MESSAGE_STRUCT(NameResponse)
TYPE_TO_MESSAGE_STRUCT(SearchRequest)
TYPE_TO_MESSAGE_STRUCT(SearchResponse)
TYPE_TO_MESSAGE_STRUCT(Bye)
TYPE_TO_MESSAGE_STRUCT(ByeAck)
TYPE_TO_MESSAGE_STRUCT(AddRadio)
TYPE_TO_MESSAGE_STRUCT(AddRadioResponse)
TYPE_TO_MESSAGE_STRUCT(ListenConnectionRequest)
TYPE_TO_MESSAGE_STRUCT(Radios)
TYPE_TO_MESSAGE_STRUCT(Queue)
TYPE_TO_MESSAGE_STRUCT(PlaylistUpdate)

#undef TYPE_TO_MESSAGE_STRUCT

}

BOOST_FUSION_ADAPT_STRUCT
(
	dmp_library::LibraryEntry,
	(std::string, artist)
	(std::string, ascii_artist)
	(std::string, title)
	(std::string, ascii_title)
	(std::string, album)
	(std::string, ascii_album)
	(uint32_t, track)
	(uint32_t, id)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::NoMessage,
	(message::Type, type)
)

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
	(std::string, radio_name)
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
typedef std::map<std::string, message::Playlist> radios_type;

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

BOOST_FUSION_ADAPT_STRUCT(
	message::Queue,
	(message::Type, type)
	(std::string, radio)
	(std::string, queuer)
	(std::string, owner)
	(dmp_library::LibraryEntry, entry)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::PlaylistUpdate::Action,
	(message::PlaylistUpdate::Action::Type, type)
	(uint32_t, from)
	(uint32_t, to)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::PlaylistUpdate,
	(message::Type, type)
	(message::PlaylistUpdate::Action, action)
	(std::string, radio_name)
	(message::Playlist, playlist)
)
