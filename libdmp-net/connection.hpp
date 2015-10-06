#pragma once

#include "message.hpp"
#include "archive.hpp"

#include "fusion_serializer.hpp"
#include "fusion_outputter.hpp"

#include <sodium/crypto_box.h>

#include <boost/asio/buffer.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>
#include <boost/cstdint.hpp>

#include <sstream>
#include <cstdint>
#include <vector>
#include <queue>
#include <assert.h>
#include <stddef.h>
#include <functional>
#include <stdexcept>
#include <string>

class Connection {

	bool encrypt;
	void encrypted(std::function<void(bool)> cb);

	boost::asio::ip::tcp::socket socket;

	std::vector<uint8_t> encrypted_buffer;

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
			socket.get_io_service().post([this, x]{
				send_encrypted(x);
			});
		} else {
			socket.get_io_service().post([this, x]{
				send_plain(x);
			});
		}
	}
	
	template <typename Callable>
	void async_receive_type(Callable const cb)
	{
		encrypted([this, cb](bool encrypted){
			if(encrypted) {
				async_receive_encrypted_type(cb);
			} else {
				async_receive_plain_type(cb);
			}
		});
		
	}
	
	template <typename T, typename Callable>
	void async_receive(Callable const cb)
	{
		encrypted([this, cb](bool encrypted){
			if(encrypted) {
				async_receive_encrypted<T>(cb);
			} else {
				async_receive_plain<T>(cb);
			}
		});
	}

	inline std::vector<uint8_t> get_public_key() {
		return public_key;
	}
	
private:

	bool type_internal(message::Type type);
	message::Nonce::Nonce_t generate_nonce();

	void handle_nonce(message::Nonce x);
	std::function<void(message::Nonce)> handle_nonce_helper;
	
	template <typename Callable>
	void handle_internal(message::Type type, Callable const cb) {
	
		if (type == message::Type::Nonce) {
			if(!handle_nonce_helper) {
				handle_nonce_helper = [this, cb](message::Nonce x){handle_nonce(x); async_receive_type(cb);};
			}
			async_receive<message::Nonce>(handle_nonce_helper);
		}
	}
	
	template <typename T>
	std::string create_payload(T& x) const {
		std::ostringstream oss;
		OArchive oar(oss);
		message::serialize(oar, x);
		std::string content = oss.str();
		return content;
	}
	
	template <typename T>
	T create_message(std::string& payload) {
		std::istringstream iss(payload);
		IArchive iar(iss);

		return message::serialize<T>(iar);
	}
	
	void check_common_errors(std::string callee, boost::system::error_code const& ec, size_t read_bytes, std::vector<uint8_t> const& buffer) const {
		if (ec) {
			throw std::runtime_error("Error receiving size in: " + callee);
		}
		
		if(read_bytes != buffer.size()) {
			throw std::runtime_error("Unexpected number of bytes read in: " + callee);
		}
	}
	
	std::vector<uint8_t> get_cypher_text(std::vector<uint8_t> plaintext, message::Nonce::Nonce_t nonce) const {
		
		std::vector<uint8_t> cyphertext(plaintext.size() + crypto_box_macbytes());
	
		bool succes = !crypto_box_easy(
			&cyphertext[0],
			plaintext.data(),
			plaintext.size(),
			nonce.data(),
			other_public_key.data(),
			private_key.data()
		);
	
		if(!succes) {
			throw std::runtime_error("failed to encrypt type");
		}
		
		return cyphertext;
	}
	
	std::vector<uint8_t> get_plain_text(std::vector<uint8_t> cyphertext, message::Nonce::Nonce_t nonce) const {
	
		std::vector<uint8_t> plaintext(cyphertext.size() - crypto_box_macbytes());
		
		bool succes = !crypto_box_open_easy(
			&plaintext[0],
			cyphertext.data(),
			cyphertext.size(),
			nonce.data(),
			other_public_key.data(),
			private_key.data()
		);

		if(!succes) {
			throw std::runtime_error("failed to decrypt the type");
		}
		
		return plaintext;
	}

	template <typename T>
	void send_plain(T x)
	{		
		std::string payload = create_payload(x);
		uint32_t size = payload.size();

		auto type = message::message_to_type(x);
		static_assert(sizeof(decltype(type)) == 4, "Size of type variable in message struct is assumed to be 4 bytes, but is not.");
		
		uint8_t encrypted = 0;
		
		boost::asio::write(socket, boost::asio::buffer(&encrypted, 1));
		boost::asio::write(socket, boost::asio::buffer(&type, 4));
		boost::asio::write(socket, boost::asio::buffer(&encrypted, 1));
		boost::asio::write(socket, boost::asio::buffer(&size, 4));
		boost::asio::write(socket, boost::asio::buffer(payload));
	}

	template <typename T>
	void send_encrypted(T x)
	{
		auto type = message::message_to_type(x);
	
		auto send_type = [this, type](message::Nonce::Nonce_t nonce) {
			static_assert(sizeof(decltype(type)) == 4, "Size of type variable in message struct is assumed to be 4 bytes, but is not.");
	
			std::vector<uint8_t> plaintext(reinterpret_cast<uint8_t const*>(&type), reinterpret_cast<uint8_t const*>(&type)+4);
	
			std::vector<uint8_t> cypher_text = get_cypher_text(plaintext, nonce);
	
			uint8_t encrypted = 1;
			boost::asio::write(socket, boost::asio::buffer(&encrypted, 1));
			boost::asio::write(socket, boost::asio::buffer(cypher_text));
		};

		std::string payload = create_payload(x);

		auto send_payload = [this, payload](message::Nonce::Nonce_t nonce) {
			
			std::vector<uint8_t> plain_text(payload.begin(), payload.end());
	
			std::vector<uint8_t> cypher_text = get_cypher_text(plain_text, nonce);
	
			uint32_t size = cypher_text.size();
	
			static_assert(sizeof(decltype(size)) == 4, "Size of uint32_t is assumed to be 4 bytes, but is not.");
	
			uint8_t encrypted = 1;
			boost::asio::write(socket, boost::asio::buffer(&encrypted, 1));
			boost::asio::write(socket, boost::asio::buffer(&size, 4));
			boost::asio::write(socket, boost::asio::buffer(cypher_text));
		};
		
		encrypted_send_queue.push(send_type);
		encrypted_send_queue.push(send_payload);
		
		send_plain(message::Nonce(message::Nonce::Type::Request, {}));
		send_plain(message::Nonce(message::Nonce::Type::Request, {}));
	}

	template <typename Callable>
	void async_receive_plain_type(Callable const cb)
	{
		auto read_cb = [this, cb](boost::system::error_code ec, size_t read_bytes)
		{
			check_common_errors("async_receive_plain_type::type_lambda", ec, read_bytes, async_type_buffer);
			
			uint8_t const* data = async_type_buffer.data();
			message::Type type = static_cast<message::Type>(*reinterpret_cast<const message::Type_t*>(data));
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
	void async_receive_encrypted_type(Callable const cb)
	{
		auto type_cb = [this, cb](boost::system::error_code ec, size_t read_bytes)
		{
			check_common_errors("async_receive_encrypted_type::type_lambda", ec, read_bytes, async_type_buffer);
			
			auto nonce = sent_nonces.front();
			sent_nonces.pop();
			
			std::vector<uint8_t> plaintext = get_plain_text(async_type_buffer, nonce);

			uint8_t const* data = plaintext.data();
			message::Type type = static_cast<message::Type>(*reinterpret_cast<const message::Type_t*>(data));
			assert(type != message::Type::NoMessage);
			cb(type);
		};

		async_type_buffer.resize(sizeof(message::Type_t) + crypto_box_macbytes(), 0);
		boost::asio::async_read(socket, boost::asio::buffer(async_type_buffer), type_cb);
	}

	template <typename T, typename Callable>
	void async_receive_plain(Callable const cb)
	{
		auto size_cb = [this, cb](boost::system::error_code ec, size_t read_bytes)
		{
			check_common_errors("async_receive_plain::size_lambda", ec, read_bytes, async_size_buffer);

			uint8_t const* data = async_size_buffer.data();
			uint32_t size = *reinterpret_cast<const uint32_t*>(data);

			auto content_cb = [this, cb, size](boost::system::error_code ec, size_t read_bytes)
			{
				check_common_errors("async_receive_plain::content_lambda", ec, read_bytes, async_mess_buffer);

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
	void async_receive_encrypted(Callable const cb)
	{
		auto size_cb = [this, cb](boost::system::error_code ec, size_t read_bytes)
		{
			check_common_errors("async_receive_encrypted::size_lambda", ec, read_bytes, async_size_buffer);
			
			uint8_t const* data = async_size_buffer.data();
			uint32_t size = *reinterpret_cast<const uint32_t*>(data);

			auto content_cb = [this, cb, size](boost::system::error_code ec, size_t read_bytes)
			{
				check_common_errors("async_receive_encrypted::content_lambda", ec, read_bytes, async_mess_buffer);
				
				auto nonce = sent_nonces.front();
				sent_nonces.pop();

				std::vector<uint8_t> plaintext = get_plain_text(async_mess_buffer, nonce);

				std::string payload = std::string(reinterpret_cast<const char*>(plaintext.data()), plaintext.size());

				cb(create_message<T>(payload));
			};

			async_mess_buffer.resize(size, 0);
			boost::asio::async_read(socket, boost::asio::buffer(async_mess_buffer), content_cb);
		};

		async_size_buffer.resize(4, 0);
		boost::asio::async_read(socket, boost::asio::buffer(async_size_buffer), size_cb);
	};
};
