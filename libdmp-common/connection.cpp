#include "connection.hpp"

namespace dmp {

ReceiveProxy Connection::receive()
{
	return ReceiveProxy{*this};
}

}

