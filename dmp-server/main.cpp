
#include "dmp_server.hpp"

#include "core_dump.hpp"

#include <boost/asio/io_service.hpp>

#include <iostream>
#include <string>

int main(int, char**) {

#ifdef __linux
	signal(SIGINT, core_dump);
#endif

	auto io_service = std::make_shared<boost::asio::io_service>();
	auto server = std::make_shared<DmpServer>(io_service);

	std::thread server_thread(std::bind(&DmpServer::run, server.get()));

	bool stop = false;
	while(!stop)
	{
		std::string input;
		std::getline(std::cin, input);

		if (input == "stop")
		{
			stop = true;
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	server->stop();
	server_thread.join();

	return 0;
}
