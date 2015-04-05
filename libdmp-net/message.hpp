#pragma once

#include "dmp-library.hpp"
#include "playlist.hpp"
#include "radio_state.hpp"

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

#include <cstdint>

namespace message {

typedef uint32_t Type_t;

enum class Type : Type_t {
	NoMessage,
	PublicKey,
	Ping,
	Pong,
	LoginRequest,
	LoginResponse,
	RegisterRequest,
	RegisterResponse,
	SearchRequest,
	SearchResponse,
	Bye,
	ByeAck,
	AddRadio,
	RemoveRadio,
	AddRadioResponse,
	ListenConnectionRequest,
	Radios,
	Queue,
	PlaylistUpdate,
	StreamRequest,
	RadioAction,
	SenderEvent,
	ReceiverAction,
	SenderAction,
	TuneIn,
	RadioStates,
	LAST
};

inline std::ostream& operator<<(std::ostream& os, message::Type t)
{
	return os << static_cast<message::Type_t>(t);
}

struct NoMessage {
	NoMessage()
	{
		throw std::runtime_error("No NoMessage struct should be made ever.");
	}
};

struct PublicKey {
	std::vector<uint8_t> key;

	PublicKey(std::vector<uint8_t> key)
	: key(key)
	{}
};

struct Ping {
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
	: payload(random_string())
	{}
};

struct Pong
{
	std::string payload;

	Pong(std::string payload)
	: payload(payload)
	{}
};

struct LoginRequest
{
	std::string username;
	std::string password;

	LoginRequest(std::string username, std::string password)
	: username(username)
	, password(password)
	{}
};

struct LoginResponse
{
	bool succes;
	std::string reason;
	
	LoginResponse(bool succes, std::string reason)
	: succes(succes)
	, reason(reason)
	{}
};

struct RegisterRequest
{
	std::string username;
	std::string password;
	
	RegisterRequest(std::string username, std::string password)
	: username(username)
	, password(password)
	{}
};

struct RegisterResponse
{
	bool succes;
	std::string reason;
	
	RegisterResponse(bool succes, std::string reason)
	: succes(succes)
	, reason(reason)
	{}
};

struct SearchRequest
{
	std::string query;

	SearchRequest(std::string query)
	: query(query)
	{}
};

struct SearchResponse
{
	std::string origin;
	std::string query;
	std::vector<dmp_library::LibraryEntry> results;

	SearchResponse(std::string origin, std::string query, std::vector<dmp_library::LibraryEntry> results)
	: origin(origin)
	, query(query)
	, results(results)
	{}
};

struct Bye
{};

struct ByeAck
{};

struct AddRadio {
	std::string name;

	AddRadio(std::string name)
	: name(name)
	{}
};

struct RemoveRadio {
	std::string name;
	
	RemoveRadio(std::string name)
	: name(name)
	{}
};

struct AddRadioResponse {
	std::string radio_name;
	bool succes;
	std::string reason;

	AddRadioResponse(std::string radio_name, bool succes, std::string reason="")
	: radio_name(radio_name)
	, succes(succes)
	, reason(reason)
	{}
};

struct ListenConnectionRequest {
	std::string radio_name;
	uint16_t port;

	ListenConnectionRequest(std::string radio_name, uint16_t port)
	: radio_name(radio_name)
	, port(port)
	{}
};

struct Radios {
	std::map<std::string, Playlist> radios;

	Radios(std::map<std::string, Playlist> radios)
	: radios(radios)
	{}
};

struct Queue {
	std::string radio;
	std::string queuer;
	std::string owner;
	dmp_library::LibraryEntry entry;

	Queue(std::string radio, std::string queuer, std::string owner, dmp_library::LibraryEntry entry)
	: radio(radio)
	, queuer(queuer)
	, owner(owner)
	, entry(entry)
	{}
};

struct PlaylistUpdate {
	struct Action
	{
		enum class Type : Type_t
		{
			NoAction,
			Append,
			Update,
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

	Action action;
	std::string radio_name;
	Playlist playlist;

	PlaylistUpdate(Action action, std::string radio_name, Playlist playlist)
	: action(action)
	, radio_name(radio_name)
	, playlist(playlist)
	{}
};

struct StreamRequest {
	std::string radio_name;
	dmp_library::LibraryEntry entry;
	uint16_t port;
	
	StreamRequest(std::string radio_name, dmp_library::LibraryEntry entry, uint16_t port)
	: radio_name(radio_name)
	, entry(entry)
	, port(port)
	{}
};

enum class PlaybackAction : Type_t{
	NoAction,
	Stop,
	Pause,
	Play,
	Next,
	Reset
};

std::ostream& operator<<(std::ostream& os, PlaybackAction const& a);

enum class PlaybackEvent : Type_t {
	NoEvent,
	Paused
};

struct RadioAction {
	std::string radio_name;
	PlaybackAction action;

	RadioAction(std::string radio_name, PlaybackAction action)
	: radio_name(radio_name)
	, action(action)
	{}
};

struct ReceiverAction {
	std::string radio_name;
	PlaybackAction action;

	ReceiverAction(std::string radio_name, PlaybackAction action)
	: radio_name(radio_name)
	, action(action)
	{}
};

struct SenderAction {
	std::string radio_name;
	PlaybackAction action;

	SenderAction(std::string radio_name, PlaybackAction action)
	: radio_name(radio_name)
	, action(action)
	{}
};

struct SenderEvent {
	std::string radio_name;
	PlaybackEvent event;

	SenderEvent(std::string radio_name, PlaybackEvent event)
	: radio_name(radio_name)
	, event(event)
	{}
};

struct TuneIn {
	
	enum class Action {
		NoAction,
		TuneIn,
		TuneOff
	};

	std::string radio_name;
	Action action;

	TuneIn(std::string radio_name, Action action)
	: radio_name(radio_name)
	, action(action)
	{}
};

struct RadioStates {
	enum class Action {
		NoAction,
		Set,
	};

	Action action;
	std::map<std::string, RadioState> states;

	RadioStates(RadioStates::Action action, std::map<std::string, RadioState> states)
	: action(action)
	, states(states)
	{}
};

template <Type t>
struct type_to_message
{
	typedef NoMessage type;
};

inline Type message_to_type(NoMessage)
{
	return Type::NoMessage;
}

#define MESSAGE_TYPE_CONVERSION(X) \
template<>\
struct type_to_message<Type::X>\
{\
	typedef X type;\
}; \
 \
inline Type message_to_type(X) \
{ \
	return Type::X; \
}

MESSAGE_TYPE_CONVERSION(PublicKey)
MESSAGE_TYPE_CONVERSION(Ping)
MESSAGE_TYPE_CONVERSION(Pong)
MESSAGE_TYPE_CONVERSION(LoginRequest)
MESSAGE_TYPE_CONVERSION(LoginResponse)
MESSAGE_TYPE_CONVERSION(RegisterRequest)
MESSAGE_TYPE_CONVERSION(RegisterResponse)
MESSAGE_TYPE_CONVERSION(SearchRequest)
MESSAGE_TYPE_CONVERSION(SearchResponse)
MESSAGE_TYPE_CONVERSION(Bye)
MESSAGE_TYPE_CONVERSION(ByeAck)
MESSAGE_TYPE_CONVERSION(AddRadio)
MESSAGE_TYPE_CONVERSION(RemoveRadio)
MESSAGE_TYPE_CONVERSION(AddRadioResponse)
MESSAGE_TYPE_CONVERSION(ListenConnectionRequest)
MESSAGE_TYPE_CONVERSION(Radios)
MESSAGE_TYPE_CONVERSION(Queue)
MESSAGE_TYPE_CONVERSION(PlaylistUpdate)
MESSAGE_TYPE_CONVERSION(StreamRequest)
MESSAGE_TYPE_CONVERSION(RadioAction)
MESSAGE_TYPE_CONVERSION(SenderAction)
MESSAGE_TYPE_CONVERSION(SenderEvent)
MESSAGE_TYPE_CONVERSION(ReceiverAction)
MESSAGE_TYPE_CONVERSION(TuneIn)
MESSAGE_TYPE_CONVERSION(RadioStates)

#undef MESSAGE_TYPE_CONVERSION

}

BOOST_FUSION_ADAPT_STRUCT(
	message::NoMessage,
)

BOOST_FUSION_ADAPT_STRUCT(
	message::PublicKey,
	(std::vector<uint8_t>, key)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::Ping,
	(std::string, payload)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::Pong,
	(std::string, payload)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::LoginRequest,
	(std::string, username)
	(std::string, password)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::LoginResponse,
	(bool, succes)
	(std::string, reason)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::RegisterRequest,
	(std::string, username)
	(std::string, password)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::RegisterResponse,
	(bool, succes)
	(std::string, reason)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::SearchRequest,
	(std::string, query)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::SearchResponse,
	(std::string, origin)
	(std::string, query)
	(std::vector<dmp_library::LibraryEntry>, results)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::Bye,
)

BOOST_FUSION_ADAPT_STRUCT(
	message::ByeAck,
)

BOOST_FUSION_ADAPT_STRUCT(
	message::AddRadio,
	(std::string, name)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::RemoveRadio,
	(std::string, name)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::AddRadioResponse,
	(std::string, radio_name)
	(bool, succes)
	(std::string, reason)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::ListenConnectionRequest,
	(std::string, radio_name)
	(uint16_t, port)
)

//To not confuse the macro expansion about macro parameters.
typedef std::map<std::string, Playlist> RadiosType;

BOOST_FUSION_ADAPT_STRUCT(
	message::Radios,
	(RadiosType, radios)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::Queue,
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
	(message::PlaylistUpdate::Action, action)
	(std::string, radio_name)
	(Playlist, playlist)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::StreamRequest,
	(std::string, radio_name)
	(dmp_library::LibraryEntry, entry)
	(uint16_t, port)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::RadioAction,
	(std::string, radio_name)
	(message::PlaybackAction, action)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::ReceiverAction,
	(std::string, radio_name)
	(message::PlaybackAction, action)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::SenderAction,
	(std::string, radio_name)
	(message::PlaybackAction, action)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::SenderEvent,
	(std::string, radio_name)
	(message::PlaybackEvent, event)
)

BOOST_FUSION_ADAPT_STRUCT(
	message::TuneIn,
	(std::string, radio_name)
	(message::TuneIn::Action, action)
)

//To not confuse the macro expansion about macro parameters.
typedef std::map<std::string, RadioState> RadioStatesType;
BOOST_FUSION_ADAPT_STRUCT(
	message::RadioStates,
	(message::RadioStates::Action, action)
	(RadioStatesType, states)
)
