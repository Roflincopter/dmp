#include "connection.hpp"

ReceiveProxy Connection::receive()
{
	return ReceiveProxy{*this};
}
