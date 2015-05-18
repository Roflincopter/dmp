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

class Connection {

	struct ReceivePlainProxy {
		Connection& c;

		template <typename T>
		operator T()
		{
			uint32_t size;
			{
				std::vector<uint8_t> buf(sizeof(decltype(size)));
				size_t read_bytes = boost::asio::read(c.socket, boost::asio::buffer(buf));
				if(read_bytes != buf.size()) {
					throw std::runtime_error("Unexpected number of bytes read");
				}
				uint8_t* data = buf.data();
				size = *reinterpret_cast<const uint32_t*>(data);
			}

			std::string content;
			{
				std::vector<uint8_t> buf(size);
				size_t read_bytes = boost::asio::read(c.socket, boost::asio::buffer(buf));
				if(read_bytes != size) {
					throw std::runtime_error("Unexpected number of bytes read");
				}
				uint8_t* data = buf.data();
				content = std::string(reinterpret_cast<const char*>(data), size);
			}

			std::istringstream iss(content);
			IArchive iar(iss);

			T t = message::serialize<T>(iar);

			return t;
		}
	};

	struct ReceiveEncryptedProxy {
		Connection& c;

		template <typename T>
		operator T()
		{
			std::vector<uint8_t> nonce;
			{
				nonce.resize(crypto_box_noncebytes(), 0);
				size_t read_bytes = boost::asio::read(c.socket, boost::asio::buffer(nonce));
				if(read_bytes != nonce.size()) {
					throw std::runtime_error("Unexpected number of bytes read");
				}
			}

			uint32_t size;
			{
				std::vector<uint8_t> buf(sizeof(decltype(size)));
				size_t read_bytes = boost::asio::read(c.socket, boost::asio::buffer(buf));
				if(read_bytes != buf.size()) {
					throw std::runtime_error("Unexpected number of bytes read");
				}
				uint8_t* data = buf.data();
				size = *reinterpret_cast<const uint32_t*>(data);
			}

			std::vector<uint8_t> content;
			{
				content.resize(size, 0);
				size_t read_bytes = boost::asio::read(c.socket, boost::asio::buffer(content));
				if(read_bytes != content.size()) {
					throw std::runtime_error("Unexpected number of bytes read");
				}
			}

			std::vector<uint8_t> data(size - crypto_box_macbytes(), 0);
			bool succes = !crypto_box_open_easy(
				&data[0],
				content.data(),
				content.size(),
				nonce.data(),
				c.other_public_key.data(),
				c.private_key.data()
			);

			if(!succes) {
				throw std::runtime_error("Failed to decrypt message");
			}
			
			std::stringstream ss(std::string(reinterpret_cast<const char*>(data.data()), data.size()));
			IArchive iar(ss);
			return message::serialize<T>(iar);
		}
	};
	
	struct ReceiveProxy {
		Connection& c;
		
		template <typename T>
		operator T()
		{
			if(c.encrypted) {
				return ReceiveEncryptedProxy{c};
			} else {
				return ReceivePlainProxy{c};
			}
		}
	};

	bool encrypted;

	boost::asio::ip::tcp::socket socket;

	std::vector<uint8_t> async_type_nonce_buffer;
	std::vector<uint8_t> async_type_buffer;

	std::vector<uint8_t> async_mess_nonce_buffer;
	std::vector<uint8_t> async_size_buffer;
	std::vector<uint8_t> async_mess_buffer;

	std::vector<uint8_t> private_key;
	std::vector<uint8_t> public_key;
	std::vector<uint8_t> other_public_key;

	boost::mutex send_mutex;

public:

	Connection(boost::asio::ip::tcp::socket&& socket);
	Connection(Connection&& that);
	Connection& operator=(Connection&& that);
	~Connection();

	template <typename T>
	void send(T x) {
		if(encrypted) {
			send_encrypted(x);
		} else {
			send_plain(x);
		}
	}
	
	message::Type receive_type();
	
	template <typename Callable>
	void async_receive_type(Callable const& cb)
	{
		if(encrypted) {
			async_receive_encrypted_type(cb);
		} else {
			async_receive_plain_type(cb);
		}
	}
	
	ReceiveProxy receive();
	
	template <typename T, typename Callable>
	void async_receive(Callable const& cb)
	{
		if(encrypted) {
			async_receive_encrypted<T>(cb);
		} else {
			async_receive_plain<T>(cb);
		}
	}
	
private:

	template <typename T>
	void send_plain(T x)
	{
		boost::interprocess::scoped_lock<boost::mutex> l(send_mutex);
		std::ostringstream oss;
		OArchive oar(oss);
		message::serialize(oar, x);
		std::string content = oss.str();
		uint32_t size = content.size();

		auto type = message::message_to_type(x);
		static_assert(sizeof(decltype(type)) == 4, "Size of type variable in message struct is assumed to be 4 bytes, but is not.");
		boost::asio::write(socket, boost::asio::buffer(&type, 4));
		boost::asio::write(socket, boost::asio::buffer(&size, 4));
		boost::asio::write(socket, boost::asio::buffer(content));
	}

	template <typename T>
	void send_encrypted(T x)
	{
		boost::interprocess::scoped_lock<boost::mutex> l(send_mutex);

		auto type = message::message_to_type(x);
		static_assert(sizeof(decltype(type)) == 4, "Size of type variable in message struct is assumed to be 4 bytes, but is not.");

		std::vector<uint8_t> type_cypher(sizeof(decltype(type)) + crypto_box_macbytes());
		std::vector<uint8_t> nonce1(crypto_box_noncebytes(), 0);

		randombytes(&nonce1[0], nonce1.size());

		bool succes1 = !crypto_box_easy(
			&type_cypher[0],
			reinterpret_cast<const uint8_t*>(&type),
			sizeof(decltype(type)),
			nonce1.data(),
			other_public_key.data(),
			private_key.data()
		);

		if(!succes1) {
			throw std::runtime_error("failed to encrypt type");
		}

		boost::asio::write(socket, boost::asio::buffer(nonce1));
		boost::asio::write(socket, boost::asio::buffer(type_cypher));

		std::stringstream ss;
		OArchive oar(ss);
		message::serialize(oar, x);

		std::string tmp_str = ss.str();
		std::vector<uint8_t> content(tmp_str.begin(), tmp_str.end());

		std::vector<uint8_t> message_cypher(content.size() + crypto_box_macbytes());
		std::vector<uint8_t> nonce2(crypto_box_noncebytes(), 0);

		randombytes(&nonce2[0], nonce2.size());

		bool succes2 = !crypto_box_easy(
			&message_cypher[0],
			content.data(),
			content.size(),
			nonce2.data(),
			other_public_key.data(),
			private_key.data()
		);

		if(!succes2) {
			throw std::runtime_error("failed to encrypt message");
		}

		uint32_t size = message_cypher.size();

		static_assert(sizeof(decltype(size)) == 4, "Size of uint32_t is assumed to be 4 bytes, but is not.");

		boost::asio::write(socket, boost::asio::buffer(nonce2));
		boost::asio::write(socket, boost::asio::buffer(&size, 4));
		boost::asio::write(socket, boost::asio::buffer(message_cypher));
	}

	message::Type receive_plain_type();
	message::Type receive_encrypted_type();

	template <typename Callable>
	void async_receive_plain_type(Callable const& cb)
	{
		async_type_buffer.resize(4, 0);

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
			cb(type);
		};

		boost::asio::async_read(socket, boost::asio::buffer(async_type_buffer), read_cb);
	}

	template <typename Callable>
	void async_receive_encrypted_type(Callable const& cb)
	{
		auto type_nonce_cb = [this, cb](boost::system::error_code ec, size_t read_bytes)
		{
			if(ec) {
				if(ec.value() == boost::system::errc::operation_canceled) {
					return;
				}
				throw std::runtime_error("Failed to receive nonce of type");
			}

			if(read_bytes != async_type_nonce_buffer.size()) {
				throw std::runtime_error("Unexpected number of bytes read");
			}
			
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

				bool succes = !crypto_box_open_easy(
					&type_buf[0],
					async_type_buffer.data(),
					async_type_buffer.size(),
					async_type_nonce_buffer.data(),
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
		};

		async_type_nonce_buffer.resize(crypto_box_noncebytes(), 0);
		boost::asio::async_read(socket, boost::asio::buffer(async_type_nonce_buffer), type_nonce_cb);
	}

	ReceivePlainProxy receive_plain();
	ReceiveEncryptedProxy receive_encrypted();

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
		auto nonce_cb = [this, &cb](boost::system::error_code ec, size_t read_bytes)
		{
			if (ec) {
				throw std::runtime_error("Error receiving message nonce");
			}

			if(read_bytes != async_mess_nonce_buffer.size()) {
				throw std::runtime_error("Unexpected number of bytes read");
			}
			
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

					bool succes = 0 == crypto_box_open_easy(
						&data[0],
						async_mess_buffer.data(),
						async_mess_buffer.size(),
						async_mess_nonce_buffer.data(),
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

		async_mess_nonce_buffer.resize(crypto_box_noncebytes(), 0);
		boost::asio::async_read(socket, boost::asio::buffer(async_mess_nonce_buffer), nonce_cb);
	}
};


