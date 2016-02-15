#pragma once

#include "message.hpp"

#include <boost/variant/variant.hpp>
#include <boost/version.hpp>
#include <boost/variant/get.hpp>
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
	std::shared_ptr<boost::asio::strand> strand;

	DmpCallbacks(std::function<void()> refresher, Callbacks_t initial_callbacks, std::shared_ptr<boost::asio::io_service> io_service)
	: callbacks(initial_callbacks)
	, refresher(refresher)
	, strand(std::make_shared<boost::asio::strand>(*io_service))
	{}

	template <typename T>
	void operator()(T message) const
	{
		auto it = callbacks.find(message_to_type(message));

		if (it != callbacks.cend()) { 

			if(
//In boost 1.58 they added compile time checks for boost::get.
//But because i generate code that won't be called it triggers compilation errors.
//So use relaxed_get from 1.58 onwards.
#if ( BOOST_VERSION / 100000 == 1 ) && ( BOOST_VERSION / 100 % 1000 >= 58 ) 
				auto f = boost::relaxed_get<CB<T>>(it->second)
#else
				auto f = boost::get<CB<T>>(it->second)
#endif
			) {
				strand->post([f, message](){f(message);});
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
		strand->get_io_service().stop();
	}
};

}
