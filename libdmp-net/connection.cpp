#include "connection.hpp"

#include <sodium/randombytes.h>

#include <algorithm>

Connection::Connection(boost::asio::ip::tcp::socket &&socket)
	: encrypt(false)
	, socket(std::move(socket))
	, async_type_buffer()
	, async_size_buffer()
	, async_mess_buffer()
	, private_key(crypto_box_secretkeybytes(), 0)
	, public_key(crypto_box_publickeybytes(), 0)
	, other_public_key(crypto_box_publickeybytes(), 0)
	, encrypted_send_queue()
	, sent_nonces()
{}

Connection::Connection(Connection&& that)
	: encrypt(std::move(that.encrypt))
	, socket(std::move(that.socket))
	, async_type_buffer()
	, async_size_buffer()
	, async_mess_buffer()
	, private_key(std::move(that.private_key))
	, public_key(std::move(that.public_key))
	, other_public_key(std::move(that.other_public_key))
	, encrypted_send_queue(std::move(that.encrypted_send_queue))
	, sent_nonces(std::move(that.sent_nonces))
{}

Connection& Connection::operator=(Connection&& that){
	std::swap(encrypt, that.encrypt);
	std::swap(socket, that.socket);
	std::swap(private_key, that.private_key);
	std::swap(public_key, that.public_key);
	std::swap(other_public_key, that.other_public_key);
	std::swap(encrypted_send_queue, that.encrypted_send_queue);
	std::swap(sent_nonces, that.sent_nonces);
	return *this;
}

Connection::~Connection()
{
	socket.close();
}

void Connection::set_our_keys(std::vector<uint8_t> priv, std::vector<uint8_t> pub)
{
	private_key = priv;
	public_key = pub;
}

void Connection::set_their_key(std::vector<uint8_t> opub)
{
	other_public_key = opub;
	start_encryption();
}

void Connection::stop_encryption() {
	encrypt = false;
}

void Connection::start_encryption(){
	encrypt = true;
}

bool Connection::type_internal(message::Type type) {
	return type == message::Type::Nonce;
}

message::Nonce::Nonce_t Connection::generate_nonce()
{
	message::Nonce::Nonce_t nonce;
	randombytes(&nonce[0], nonce.size());
	return nonce;
}

void Connection::handle_nonce(message::Nonce x) {
	if(x.type == message::Nonce::Type::Delivery) {
		auto f = encrypted_send_queue.front();
		f(x.nonce);
		encrypted_send_queue.pop();
	}
	if(x.type == message::Nonce::Type::Request) {
		auto nonce = generate_nonce();
		sent_nonces.push(nonce);
		send_plain(message::Nonce(message::Nonce::Type::Delivery, nonce));
	}
}

void Connection::encrypted(std::function<void(bool)> cb)
{
	encrypted_buffer.resize(1, 0);
	boost::asio::async_read(socket, boost::asio::buffer(encrypted_buffer), [this, cb](boost::system::error_code ec, size_t read_bytes) {
	
		if(ec) {
			throw std::runtime_error("Failed to read encryption sentinel");
		}
	
		if(read_bytes != encrypted_buffer.size()) {
			throw std::runtime_error("Unexpected number of bytes read");
		}
	
		assert(encrypted_buffer[0] == 1 || encrypted_buffer[0] == 0);
	
		cb(encrypted_buffer[0] != 0);
	});
}