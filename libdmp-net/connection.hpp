#pragma once

#include "debug_macros.hpp"
#include "message.hpp"
#include "message_callbacks.hpp"

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

	struct ReceiveProxy {
		Connection& c;

		template <typename T>
		operator T()
		{
			uint32_t size;
			{
				std::vector<uint8_t> buf(sizeof(decltype(size)));
				size_t read_bytes = boost::asio::read(c.socket, boost::asio::buffer(buf));
				assert(read_bytes == buf.size());
				uint8_t* data = buf.data();
				size = *reinterpret_cast<const uint32_t*>(data);
			}

			std::string content;
			{
				std::vector<uint8_t> buf(size);
				size_t read_bytes = boost::asio::read(c.socket, boost::asio::buffer(buf));
				assert(read_bytes == size);
				uint8_t* data = buf.data();
				content = std::string(reinterpret_cast<const char*>(data), size);
			}

			std::istringstream iss(content);
			boost::archive::text_iarchive iar(iss);

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
				assert(read_bytes == nonce.size());
			}

			uint32_t size;
			{
				std::vector<uint8_t> buf(sizeof(decltype(size)));
				size_t read_bytes = boost::asio::read(c.socket, boost::asio::buffer(buf));
				assert(read_bytes == buf.size());
				uint8_t* data = buf.data();
				size = *reinterpret_cast<const uint32_t*>(data);
			}

			std::vector<uint8_t> content;
			{
				content.resize(size, 0);
				size_t read_bytes = boost::asio::read(c.socket, boost::asio::buffer(content));
				assert(read_bytes == content.size());
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
		}
	};

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
	void send(T x)
	{
		boost::interprocess::scoped_lock<boost::mutex> l(send_mutex);
		std::ostringstream oss;
		boost::archive::text_oarchive oar(oss);
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
		std::vector<uint8_t> nonce1(crypto_box_noncebytes());

		randombytes(&nonce1[0], nonce1.size());

		bool succes1 = !crypto_box_easy(
			&type_cypher[0],
			reinterpret_cast<uint8_t*>(&type),
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
		boost::archive::text_oarchive oar(ss);
		message::serialize(oar, x);

		std::string tmp_str = ss.str();
		std::vector<uint8_t> content(tmp_str.begin(), tmp_str.end());

		std::vector<uint8_t> message_cypher(content.size() + crypto_box_macbytes());
		std::vector<uint8_t> nonce2(crypto_box_noncebytes());

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

		boost::asio::write(socket, boost::asio::buffer(nonce2));
		boost::asio::write(socket, boost::asio::buffer(&size, 4));
		boost::asio::write(socket, boost::asio::buffer(message_cypher));
	}

	message::Type receive_type();
	message::Type receive_encrypted_type();

	template <typename Callable>
	void async_receive_type(Callable const& cb)
	{
		async_type_buffer.resize(4, 0);

		auto read_cb = [this, cb](boost::system::error_code ec, size_t bytes_transfered)
		{
			message::Type type = message::Type::NoMessage;

			if(ec)
			{
				throw std::runtime_error("Error in ansyc_receive_type_handler");
			}

			assert(bytes_transfered == 4);
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
		auto type_nonce_cb = [this, cb](boost::system::error_code ec, size_t bytes_transfered)
		{
			if(ec) {
				if(ec.value() == boost::system::errc::operation_canceled) {
					return;
				}
				throw std::runtime_error("Failed to receive nonce of type");
			}

			assert(bytes_transfered == async_type_nonce_buffer.size());

			auto type_cb = [this, cb](boost::system::error_code ec, size_t bytes_transfered)
			{
				message::Type type = message::Type::NoMessage;

				if(ec) {
					throw std::runtime_error("Failed to receive type");
				}

				assert(bytes_transfered == async_type_buffer.size());

				std::vector<uint8_t> type_buf(sizeof(message::Type_t));

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

	ReceiveProxy receive();
	ReceiveEncryptedProxy receive_encrypted();

	template <typename T, typename Callable>
	void async_receive(Callable const& cb)
	{
		auto size_cb = [this, &cb](boost::system::error_code ec, size_t bytes_transfered)
		{
			if (ec)
			{
				throw std::runtime_error("Error receiving size in size_cb lambda");
			}

			assert(bytes_transfered == 4);

			uint8_t const* data = async_size_buffer.data();
			uint32_t size = *reinterpret_cast<const uint32_t*>(data);

			auto content_cb = [this, &cb, size](boost::system::error_code ec, size_t bytes_transfered)
			{
				if (ec)
				{
					throw std::runtime_error("Error receiving size in content_cb lambda");
				}

				assert(bytes_transfered == size);

				uint8_t const* data = async_mess_buffer.data();
				std::string content = std::string(reinterpret_cast<const char*>(data), size);

				std::istringstream iss(content);
				boost::archive::text_iarchive iar(iss);

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
		auto nonce_cb = [this, &cb](boost::system::error_code ec, size_t bytes_transfered)
		{
			if (ec) {
				throw std::runtime_error("Error receiving message nonce");
			}

			assert(bytes_transfered == async_mess_nonce_buffer.size());

			auto size_cb = [this, &cb](boost::system::error_code ec, size_t bytes_transfered)
			{
				if (ec) {
					throw std::runtime_error("Error receiving size in size_cb lambda");
				}

				assert(bytes_transfered == 4);

				uint8_t const* data = async_size_buffer.data();
				uint32_t size = *reinterpret_cast<const uint32_t*>(data);

				auto content_cb = [this, &cb, size](boost::system::error_code ec, size_t bytes_transfered)
				{
					if (ec) {
						throw std::runtime_error("Error receiving size in content_cb lambda");
					}

					assert(bytes_transfered == size);

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
					boost::archive::text_iarchive iar(iss);

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


