#pragma once

#include <cstdint>
#include <string>

class DmpSender
{
	std::string host;
	uint16_t port;

	void cleanup();
public:
	DmpSender();
	DmpSender(std::string host, uint16_t port, std::string file);

	void run();
};
