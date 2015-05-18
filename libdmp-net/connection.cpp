#include "connection.hpp"

Connection::Connection(boost::asio::ip::tcp::socket &&socket)
	: encrypted(false)
	, socket(std::move(socket))
	, async_type_nonce_buffer()
	, async_type_buffer()
	, async_mess_nonce_buffer()
	, async_size_buffer()
	, async_mess_buffer()
	, private_key(crypto_box_secretkeybytes(), 0)
	, public_key(crypto_box_publickeybytes(), 0)
	, other_public_key(crypto_box_publickeybytes(), 0)
	, send_mutex()
{
	crypto_box_keypair(&public_key[0], &private_key[0]);
	send(message::PublicKey(public_key));
	
	auto type = receive_type();
	if(type != message::Type::PublicKey) {
		throw std::runtime_error("Failed to setup a secure connection");
	}

	message::PublicKey x = receive();
	other_public_key.swap(x.key);
	encrypted = true;
}

Connection::Connection(Connection&& that)
	: encrypted(std::move(that.encrypted))
	, socket(std::move(that.socket))
	, async_type_nonce_buffer()
	, async_type_buffer()
	, async_mess_nonce_buffer()
	, async_size_buffer()
	, async_mess_buffer()
	, private_key(std::move(that.private_key))
	, public_key(std::move(that.public_key))
	, other_public_key(std::move(that.other_public_key))
	, send_mutex()
{}

Connection& Connection::operator=(Connection&& that){
	std::swap(encrypted, that.encrypted);
	std::swap(socket, that.socket);
	std::swap(private_key, that.private_key);
	std::swap(public_key, that.public_key);
	std::swap(other_public_key, that.other_public_key);
	return *this;
}

Connection::~Connection()
{
	socket.close();
}

message::Type Connection::receive_type() {
	if(encrypted) {
		return receive_encrypted_type();
	} else {
		return receive_plain_type();
	}
}

message::Type Connection::receive_plain_type()
{
	message::Type type = message::Type::NoMessage;

	boost::system::error_code ec;
	std::vector<uint8_t> buf(4);
	size_t read_bytes = boost::asio::read(socket, boost::asio::buffer(buf), ec);
	if(ec) {
		throw std::runtime_error("Receive type failed");
	}
	
	if(read_bytes != buf.size()) {
		throw std::runtime_error("Unexpected number of bytes read");
	}
	uint8_t const* data = buf.data();
	type = static_cast<message::Type>(*reinterpret_cast<const message::Type_t*>(data));
	return type;
}

message::Type Connection::receive_encrypted_type()
{
	boost::system::error_code ec;

	std::vector<uint8_t> nonce(crypto_box_noncebytes());
	size_t read_bytes1 = boost::asio::read(socket, boost::asio::buffer(nonce));
	if(ec) {
		throw std::runtime_error("receive_type: nonce retreival failed");
	}
	
	if(read_bytes1 != nonce.size()) {
		throw std::runtime_error("Unexpected number of bytes read");
	}

	ec.clear();

	message::Type type = message::Type::NoMessage;
	std::vector<uint8_t> cypherbuf(sizeof(decltype(type)) + crypto_box_macbytes());
	size_t read_bytes2 = boost::asio::read(socket, boost::asio::buffer(cypherbuf), ec);
	if(ec) {
		throw std::runtime_error("Receive type failed");
	}
	
	if(read_bytes2 != cypherbuf.size()) {
		throw std::runtime_error("Unexpected number of bytes read");
	}

	std::vector<uint8_t> content(sizeof(decltype(type)));

	bool succes = 0 == crypto_box_open_easy(
			&content[0],
			cypherbuf.data(),
			cypherbuf.size(),
			nonce.data(),
			other_public_key.data(),
			private_key.data()
			);

	if(!succes) {
		throw std::runtime_error("failed to decrypt type");
	}

	type = static_cast<message::Type>(*reinterpret_cast<const message::Type_t*>(content.data()));
	return type;
}

Connection::ReceiveProxy Connection::receive()
{
	return ReceiveProxy{*this};
}

Connection::ReceiveEncryptedProxy Connection::receive_encrypted()
{
	return ReceiveEncryptedProxy{*this};
}
