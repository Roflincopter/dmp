#pragma once

#include "message.hpp"

#include <boost/variant/variant.hpp>
#include <boost/version.hpp>
#include <boost/variant/get.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/strand.hpp>

#include <stdexcept>
#include <functional>
#include <map>
#include <string>
#include <typeinfo>

namespace message {

template <typename T>
struct continuation {
	template<typename F>
	void operator()(F const& continuation) {
		continuation();
	}
};

template <>
struct continuation<message::ByeAck> {
	template<typename F>
	void operator()(F const&) {}
};

template <>
struct continuation<message::Bye> {
	template<typename F>
	void operator()(F const&) {}
};

struct DmpCallbacks {
	template <typename T> using CB = std::function<void(T)>;
	
	using CallBackType = boost::variant<
		  CB<message::Ping>
		, CB<message::Pong>
		, CB<message::PublicKey>
		, CB<message::Nonce>
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
		, CB<message::PlaylistUpdate>
		, CB<message::StreamRequest>
		, CB<message::RadioAction>
		, CB<message::ReceiverAction>
		, CB<message::SenderAction>
		, CB<message::SenderEvent>
		, CB<message::TuneIn>
		, CB<message::RadioStates>
		, CB<message::Disconnected>
	>;
	
	typedef std::map<message::Type, CallBackType> Callbacks_t;
	
	Callbacks_t callbacks;
	std::function<void()> refresher;
	std::shared_ptr<boost::asio::io_context> io_context;
	std::shared_ptr<boost::asio::strand<boost::asio::io_context::executor_type>> strand;

	DmpCallbacks(std::function<void()> refresher, Callbacks_t initial_callbacks, std::shared_ptr<boost::asio::io_context> io_context)
	: callbacks(initial_callbacks)
	, refresher(refresher)
	, io_context(io_context)
	, strand(std::make_shared<boost::asio::strand<boost::asio::io_context::executor_type>>(io_context->get_executor()))
	{}

	template <typename T>
	void operator()(T message) const
	{
		auto it = callbacks.find(message_to_type(message));

		if (it != callbacks.cend()) { 

			if(
//Generated code that is never called would trip boost::get's compile time
//checks, so use relaxed_get.
				auto f = boost::relaxed_get<CB<T>>(it->second)
			) {
				boost::asio::post(*strand, [f, message](){f(message);});
				continuation<T>()(refresher);
			} else {
				throw std::runtime_error("Empty functor as callback detected");
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
	
	void stop()
	{
		io_context->stop();
	}
};

}
