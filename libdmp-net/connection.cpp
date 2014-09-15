#include "connection.hpp"

Connection::ReceiveEncryptedProxy Connection::receive_encrypted()
{
	return ReceiveEncryptedProxy{*this};
}

Connection::ReceiveProxy Connection::receive()
{
	return ReceiveProxy{*this};
}
