#pragma once

#include "dmp-library.hpp"

#include <string>

struct DmpServerInterface {
	
	virtual void order_stream(std::string client, dmp_library::LibraryEntry entry, uint16_t port) = 0;
	
};
