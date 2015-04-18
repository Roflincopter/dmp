#pragma once

#include "message.hpp"
#include "index_list.hpp"
#include "connection.hpp"

#include <boost/variant.hpp>
#include <boost/variant/get.hpp>

#include <stdexcept>

namespace message {

struct DmpCallbacks {
	template <typename T> using CB = std::function<bool(T)>;
	
	using CallBackType = boost::variant<
		  CB<message::Ping>
		, CB<message::Pong>
		, CB<message::LoginRequest>
		, CB<message::LoginResponse>
		, CB<message::RegisterRequest>
		, CB<message::RegisterResponse>
		, CB<message::SearchRequest>
		, CB<message::SearchResponse>
		, CB<message::Bye>
		, CB<message::ByeAck>
		, CB<message::AddRadio>
		, CB<message::RemoveRadio>
		, CB<message::AddRadioResponse>
		, CB<message::ListenConnectionRequest>
		, CB<message::Radios>
		, CB<message::UnQueue>
		, CB<message::PlaylistUpdate>
		, CB<message::StreamRequest>
		, CB<message::RadioAction>
		, CB<message::ReceiverAction>
		, CB<message::SenderAction>
		, CB<message::SenderEvent>
		, CB<message::TuneIn>
		, CB<message::RadioStates>
	>;
	
	typedef std::map<message::Type, CallBackType> Callbacks_t;
	
	Callbacks_t callbacks;
	std::function<void()> refresher;

	DmpCallbacks(std::function<void()> refresher, Callbacks_t initial_callbacks)
	: callbacks(initial_callbacks)
	, refresher(refresher)
	{}

	template <typename T>
	void operator()(T message) const
	{
		auto it = callbacks.find(message_to_type(message));

		if (it != callbacks.cend()) {
			auto f = boost::get<CB<T>>(it->second);
			if(f(message)) {
				refresher();
			}
		} else {
			throw std::runtime_error("Requested callback type was not found in callbacks: " + std::string(typeid(message).name()) + " " + std::to_string(static_cast<message::Type_t>(message_to_type(message))));
		}
	}

	template <typename Function>
	DmpCallbacks& set(message::Type t, Function x)
	{
		callbacks[t] = x;
		return *this;
	}
	
	DmpCallbacks& unset(message::Type t)
	{
		callbacks.erase(t);
		return *this;
	}
	
	void clear()
	{
		callbacks.clear();
	}
};

}
