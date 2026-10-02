
#include "dmp_server.hpp"

#include <sodium/core.h>

#include <iostream>
#include <string>
#include <chrono>
#include <functional>
#include <memory>
#include <ratio>
#include <thread>

int main(int, char**) {

	if(sodium_init() < 0) {
		std::cerr << "Failed to initialize libsodium" << std::endl;
		return 1;
	}

#ifdef __linux
	//signal(SIGINT, core_dump);
#endif
	auto server = std::make_shared<DmpServer>();
	server->read_database();

	std::thread server_thread(std::bind(&DmpServer::run, server.get()));

	bool stop = false;
	while(!stop)
	{
		std::string input;
		std::getline(std::cin, input);

		if (input == "stop")
		{
			stop = true;
		} else if (input == "graph") {
			server->gstreamer_debug("command");
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	server->stop();
	server_thread.join();
	return 0;
}
