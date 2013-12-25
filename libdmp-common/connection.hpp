#pragma once

#include <sstream>
#include <memory>
#include <cstdint>
#include <vector>

#include <boost/asio.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/bind.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/thread/mutex.hpp>

#include "message.hpp"
#include "message_serializer.hpp"
#include "message_callbacks.hpp"

namespace dmp {

using namespace boost::asio::ip;

struct ReceiveProxy;

class Connection {

	friend class ReceiveProxy;

	tcp::socket socket;
	boost::mutex send_mutex;

	std::vector<uint8_t> async_type_buffer{};
	std::vector<uint8_t> async_size_buffer{};
	std::vector<uint8_t> async_mess_buffer{};


public:
	std::shared_ptr<boost::asio::io_service> io_service;

	Connection(std::shared_ptr<boost::asio::io_service> io_service, tcp::socket&& socket)
	: socket(std::move(socket))
	, send_mutex()
	, io_service(io_service)
	{}

	Connection(Connection&& that)
	: socket(std::move(that.socket))
	, send_mutex()
	, io_service(std::move(that.io_service))
	{}

	Connection& operator=(Connection&& that){
		std::swap(socket, that.socket);
		std::swap(io_service, that.io_service);
		return *this;
	}

	~Connection()
	{
		socket.close();
	}

	template <typename T>
	void send(T x)
	{
		boost::mutex::scoped_lock(send_mutex);
		std::ostringstream oss;
		boost::archive::text_oarchive oar(oss);
		message::serialize(oar, x);
		std::string content = oss.str();
		uint32_t size = content.size();

		boost::asio::write(socket, boost::asio::buffer(&x.type, 4));
		boost::asio::write(socket, boost::asio::buffer(&size, 4));
		boost::asio::write(socket, boost::asio::buffer(content.data(), size));
	}

	message::Type receive_type()
	{
		message::Type type = message::Type::NoMessage;

		boost::system::error_code ec;
		std::vector<uint8_t> buf(4);
		size_t read_bytes = boost::asio::read(socket, boost::asio::buffer(buf), ec);
		assert(read_bytes == 4);
		const unsigned char* data = buf.data();
		type = static_cast<message::Type>(*reinterpret_cast<const uint32_t*>(data));
		return type;
	}

	void async_receive_type(std::function<void(message::Type)> cb)
	{
		async_type_buffer.resize(4, 0);

		auto read_cb = [this, cb](boost::system::error_code ec, size_t bytes_transfered)
		{
			if(ec)
			{
				throw std::runtime_error("Error in ansyc_receive_type_handler");
			}

			assert(bytes_transfered == 4);
			const unsigned char* data = async_type_buffer.data();
			message::Type type = static_cast<message::Type>(*reinterpret_cast<const uint32_t*>(data));
			assert(type != message::Type::NoMessage);
			cb(type);
		};

		boost::asio::async_read(socket, boost::asio::buffer(async_type_buffer), read_cb);
	}

	ReceiveProxy receive();

	template <typename T, typename Callable>
	void async_receive(Callable cb)
	{
		auto size_cb = [this, cb](boost::system::error_code ec, size_t bytes_transfered)
		{
			if (ec)
			{
				throw std::runtime_error("Error receiving size in size_cb lambda");
			}

			assert(bytes_transfered == 4);

			uint8_t* data = async_size_buffer.data();
			uint32_t size = *reinterpret_cast<const uint32_t*>(data);

			auto content_cb = [this, cb, size](boost::system::error_code ec, size_t bytes_transfered)
			{
				if (ec)
				{
					throw std::runtime_error("Error receiving size in content_cb lambda");
				}

				assert(bytes_transfered == size);

				uint8_t* data = async_mess_buffer.data();
				std::string content = std::string(reinterpret_cast<const char*>(data), size);

				std::istringstream iss(content);
				boost::archive::text_iarchive iar(iss);

				T t;
				message::serialize(iar, t);
				cb(t);
			};

			async_mess_buffer.resize(size, 0);
			boost::asio::async_read(socket, boost::asio::buffer(async_mess_buffer), content_cb);
		};

		async_size_buffer.resize(4, 0);
		boost::asio::async_read(socket, boost::asio::buffer(async_size_buffer), size_cb);
	}
};

struct ReceiveProxy {
	Connection& c;

	template <typename T>
	operator T()
	{
		uint32_t size;
		std::string content;

		{
			std::vector<uint8_t> buf(4);
			size_t read_bytes = boost::asio::read(c.socket, boost::asio::buffer(buf));
			assert(read_bytes == 4);
			const unsigned char* data = buf.data();
			size = *reinterpret_cast<const uint32_t*>(data);
		}
		{
			std::vector<uint8_t> buf(size);
			size_t read_bytes = boost::asio::read(c.socket, boost::asio::buffer(buf));
			assert(read_bytes == size);
			const unsigned char* data = buf.data();
			content = std::string(reinterpret_cast<const char*>(data), size);
		}

		std::istringstream iss(content);
		boost::archive::text_iarchive iar(iss);

		T t;
		message::serialize(iar, t);

		return t;
	}
};

}
