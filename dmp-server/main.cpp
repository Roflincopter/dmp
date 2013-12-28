
#include "dmp_server.hpp"

#include "dmp-library.hpp"
#include "accept.hpp"

#include <boost/thread.hpp>

#include <iostream>
#include <string>


#include "dmp_radio.hpp"

int main(int, char**) {

	//std::cout << "Dmp server starting." << std::endl;
	//DmpRadio radio(2000, 2001);
	//return 0;

	DmpServer server;

	std::function<void(dmp::Connection&&)> f = [&](dmp::Connection&& x){
		server.add_connection(std::move(x));
	};

	std::thread server_thread(std::bind(&DmpServer::run, std::ref(server)));

	boost::asio::io_service io_service;
	std::thread accept_thread(std::bind(dmp::accept_loop, 1337, std::ref(io_service), f));

	bool stop = false;
	while(!stop)
	{
		std::string input;
		std::getline(std::cin, input);

		if (input == "stop")
		{
			stop = true;
		}
		boost::this_thread::sleep(boost::posix_time::seconds(1));
	}

	io_service.stop();
	accept_thread.join();

	return 0;
}
