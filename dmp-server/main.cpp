
#include "dmp_server.hpp"

#include "dmp-library.hpp"
#include "accept.hpp"

#include <boost/asio/io_service.hpp>

#include <iostream>
#include <string>


#include "dmp_radio.hpp"

int main(int, char**) {

	//std::cout << "Dmp server starting." << std::endl;
	//DmpRadio radio(2000, 2001);
	//return 0;

	auto server = std::make_shared<DmpServer>();

	std::function<void(Connection&&)> f = [&](Connection&& x){
		try {
			server->add_connection(std::move(x));
		} catch(std::exception &e) {
			std::cerr << "Failed to initialize connection: " << e.what() << std::endl;
		}
	};

	std::thread server_thread(std::bind(&DmpServer::run, std::ref(*server)));

	boost::asio::io_service io_service;
	std::thread accept_thread(std::bind(accept_loop, 1337, std::ref(io_service), f));

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

	io_service.stop();
	accept_thread.join();
	server->stop();
	server_thread.join();

	return 0;
}
