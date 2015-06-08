#pragma once

#include "debug_macros.hpp"
#include "message.hpp"
#include "message_callbacks.hpp"
#include "archive.hpp"

#include "fusion_serializer.hpp"
#include "fusion_outputter.hpp"

#include <sodium.h>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/placeholders.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include <sstream>
#include <iomanip>
#include <memory>
#include <cstdint>
#include <vector>
#include <queue>

class Connection {

	bool encrypt;
	bool encrypted();

	boost::asio::ip::tcp::socket socket;

	std::vector<uint8_t> async_type_buffer;

	std::vector<uint8_t> async_size_buffer;
	std::vector<uint8_t> async_mess_buffer;

	std::vector<uint8_t> private_key;
	std::vector<uint8_t> public_key;
	std::vector<uint8_t> other_public_key;
	
	std::queue<std::function<void(message::Nonce::Nonce_t)>> encrypted_send_queue;
	std::queue<message::Nonce::Nonce_t> sent_nonces;

public:

	Connection(boost::asio::ip::tcp::socket&& socket);
	Connection(Connection&& that);
	Connection& operator=(Connection&& that);
	~Connection();
	
	void set_our_keys(std::vector<uint8_t> priv, std::vector<uint8_t> pub);
	void set_their_key(std::vector<uint8_t> opub);
	void stop_encryption();
	void start_encryption();

	template <typename T>
	void send(T x) {
		
		
		if(encrypt) {
			send_encrypted(x);
		} else {
			send_plain(x);
		}
	}
	
	template <typename Callable>
	void async_receive_type(Callable const& cb)
	{
		if(encrypted()) {
			async_receive_encrypted_type(cb);
		} else {
			async_receive_plain_type(cb);
		}
	}
	
	template <typename T, typename Callable>
	void async_receive(Callable const& cb)
	{
		if(encrypted()) {
			async_receive_encrypted<T>(cb);
		} else {
			async_receive_plain<T>(cb);
		}
	}
	
private:

	bool type_internal(message::Type type);
	message::Nonce::Nonce_t generate_nonce();

	void handle_nonce(message::Nonce x);
	std::function<void(message::Nonce)> handle_nonce_helper;
	
	template <typename Callable>
	void handle_internal(message::Type type, Callable const& cb) {
	
		if (type == message::Type::Nonce) {
			if(!handle_nonce_helper) {
				handle_nonce_helper = [this, cb](message::Nonce x){handle_nonce(x); async_receive_type(cb);};
			}
			async_receive<message::Nonce>(handle_nonce_helper);
		}
	}

	template <typename T>
	void send_plain(T x)
	{
		std::ostringstream oss;
		OArchive oar(oss);
		message::serialize(oar, x);
		std::string content = oss.str();
		uint32_t size = content.size();

		auto type = message::message_to_type(x);
		static_assert(sizeof(decltype(type)) == 4, "Size of type variable in message struct is assumed to be 4 bytes, but is not.");
		
		uint8_t encrypted = 0;
		
		boost::asio::write(socket, boost::asio::buffer(&encrypted, 1));
		boost::asio::write(socket, boost::asio::buffer(&type, 4));
		boost::asio::write(socket, boost::asio::buffer(&encrypted, 1));
		boost::asio::write(socket, boost::asio::buffer(&size, 4));
		boost::asio::write(socket, boost::asio::buffer(content));
	}

	template <typename T>
	void send_encrypted(T x)
	{
		auto type = message::message_to_type(x);
	
		auto send_type = [this, type](message::Nonce::Nonce_t nonce) {
			static_assert(sizeof(decltype(type)) == 4, "Size of type variable in message struct is assumed to be 4 bytes, but is not.");
	
			std::vector<uint8_t> type_cypher(sizeof(decltype(type)) + crypto_box_macbytes());
	
			bool succes = !crypto_box_easy(
				&type_cypher[0],
				reinterpret_cast<const uint8_t*>(&type),
				sizeof(decltype(type)),
				nonce.data(),
				other_public_key.data(),
				private_key.data()
			);
	
			if(!succes) {
				throw std::runtime_error("failed to encrypt type");
			}
	
			uint8_t encrypted = 1;
			boost::asio::write(socket, boost::asio::buffer(&encrypted, 1));
			boost::asio::write(socket, boost::asio::buffer(type_cypher));
		};

		std::stringstream ss;
		OArchive oar(ss);
		message::serialize(oar, x);
		std::string payload = ss.str();

		auto send_payload = [this, payload](message::Nonce::Nonce_t nonce) {

			std::vector<uint8_t> content(payload.begin(), payload.end());
	
			std::vector<uint8_t> message_cypher(content.size() + crypto_box_macbytes());
	
			bool succes = !crypto_box_easy(
				&message_cypher[0],
				content.data(),
				content.size(),
				nonce.data(),
				other_public_key.data(),
				private_key.data()
			);
	
			if(!succes) {
				throw std::runtime_error("failed to encrypt message");
			}
	
			uint32_t size = message_cypher.size();
	
			static_assert(sizeof(decltype(size)) == 4, "Size of uint32_t is assumed to be 4 bytes, but is not.");
	
			uint8_t encrypted = 1;
			boost::asio::write(socket, boost::asio::buffer(&encrypted, 1));
			boost::asio::write(socket, boost::asio::buffer(&size, 4));
			boost::asio::write(socket, boost::asio::buffer(message_cypher));
		};
		
		encrypted_send_queue.push(send_type);
		encrypted_send_queue.push(send_payload);
		
		send_plain(message::Nonce(message::Nonce::Type::Request, {}));
		send_plain(message::Nonce(message::Nonce::Type::Request, {}));
	}

	template <typename Callable>
	void async_receive_plain_type(Callable const& cb)
	{
		auto read_cb = [this, cb](boost::system::error_code ec, size_t read_bytes)
		{
			message::Type type = message::Type::NoMessage;

			if(ec)
			{
				throw std::runtime_error("Error in ansyc_receive_type_handler");
			}
			if(read_bytes != async_type_buffer.size()) {
				throw std::runtime_error("Unexpected number of bytes read");
			}
			uint8_t const* data = async_type_buffer.data();
			type = static_cast<message::Type>(*reinterpret_cast<const message::Type_t*>(data));
			assert(type != message::Type::NoMessage);
			
			if(type_internal(type)) {
				handle_internal(type, cb);
			} else {
				cb(type);
			}
		};
		
		async_type_buffer.resize(4, 0);
		boost::asio::async_read(socket, boost::asio::buffer(async_type_buffer), read_cb);
	}

	template <typename Callable>
	void async_receive_encrypted_type(Callable const& cb)
	{
		auto type_cb = [this, cb](boost::system::error_code ec, size_t read_bytes)
		{
			message::Type type = message::Type::NoMessage;

			if(ec) {
				throw std::runtime_error("Failed to receive type");
			}

			if(read_bytes != async_type_buffer.size()) {
				throw std::runtime_error("Unexpected number of bytes read");
			}

			std::vector<uint8_t> type_buf(sizeof(message::Type_t), 0);

			auto nonce = sent_nonces.front();
			sent_nonces.pop();

			bool succes = !crypto_box_open_easy(
				&type_buf[0],
				async_type_buffer.data(),
				async_type_buffer.size(),
				nonce.data(),
				other_public_key.data(),
				private_key.data()
			);

			if(!succes) {
				throw std::runtime_error("failed to decrypt the type");
			}

			uint8_t const* data = type_buf.data();
			type = static_cast<message::Type>(*reinterpret_cast<const message::Type_t*>(data));
			assert(type != message::Type::NoMessage);
			cb(type);
		};

		async_type_buffer.resize(sizeof(message::Type_t) + crypto_box_macbytes(), 0);
		boost::asio::async_read(socket, boost::asio::buffer(async_type_buffer), type_cb);
	}

	template <typename T, typename Callable>
	void async_receive_plain(Callable const& cb)
	{
		auto size_cb = [this, &cb](boost::system::error_code ec, size_t read_bytes)
		{
			if (ec)
			{
				throw std::runtime_error("Error receiving size in size_cb lambda");
			}

			if(read_bytes != async_size_buffer.size()) {
				throw std::runtime_error("Unexpected number of bytes read");
			}

			uint8_t const* data = async_size_buffer.data();
			uint32_t size = *reinterpret_cast<const uint32_t*>(data);

			auto content_cb = [this, &cb, size](boost::system::error_code ec, size_t read_bytes)
			{
				if (ec)
				{
					throw std::runtime_error("Error receiving size in content_cb lambda");
				}

				if(read_bytes != size) {
					throw std::runtime_error("Unexpected number of bytes read");
				}

				uint8_t const* data = async_mess_buffer.data();
				std::string content = std::string(reinterpret_cast<const char*>(data), size);

				std::istringstream iss(content);
				IArchive iar(iss);

				T t = message::serialize<T>(iar);
				cb(t);
			};

			async_mess_buffer.resize(size, 0);
			boost::asio::async_read(socket, boost::asio::buffer(async_mess_buffer), content_cb);
		};

		async_size_buffer.resize(4, 0);
		boost::asio::async_read(socket, boost::asio::buffer(async_size_buffer), size_cb);
	}

	template <typename T, typename Callable>
	void async_receive_encrypted(Callable const& cb)
	{
		auto size_cb = [this, &cb](boost::system::error_code ec, size_t read_bytes)
		{
			if (ec) {
				throw std::runtime_error("Error receiving size in size_cb lambda");
			}

			if(read_bytes != async_size_buffer.size()) {
				throw std::runtime_error("Unexpected number of bytes read");
			}

			uint8_t const* data = async_size_buffer.data();
			uint32_t size = *reinterpret_cast<const uint32_t*>(data);

			auto content_cb = [this, &cb, size](boost::system::error_code ec, size_t read_bytes)
			{
				if (ec) {
					throw std::runtime_error("Error receiving size in content_cb lambda");
				}

				if(read_bytes != size) {
					throw std::runtime_error("Unexpected number of bytes read");
				}

				std::vector<uint8_t> data(size - crypto_box_macbytes());

				auto nonce = sent_nonces.front();
				sent_nonces.pop();

				bool succes = 0 == crypto_box_open_easy(
					&data[0],
					async_mess_buffer.data(),
					async_mess_buffer.size(),
					nonce.data(),
					other_public_key.data(),
					private_key.data()
				);

				if(!succes) {
					throw std::runtime_error("Failed to decrypt message");
				}

				std::string content = std::string(reinterpret_cast<const char*>(data.data()), data.size());

				std::istringstream iss(content);
				IArchive iar(iss);

				T t = message::serialize<T>(iar);
				cb(t);
			};

			async_mess_buffer.resize(size, 0);
			boost::asio::async_read(socket, boost::asio::buffer(async_mess_buffer), content_cb);
		};

		async_size_buffer.resize(4, 0);
		boost::asio::async_read(socket, boost::asio::buffer(async_size_buffer), size_cb);
	};
};


