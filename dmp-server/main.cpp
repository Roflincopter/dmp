
#include "RTSPReceiver.hpp"
#include "dmp_server.hpp"

#include "dmp-library.hpp"
#include "accept.hpp"

#include <boost/thread.hpp>

#include <iostream>
#include <string>

int main(int, char**) {

    DmpServer server;

    std::function<void(dmp::Connection&&)> f = [&](dmp::Connection&& x){
        server.add_connection(std::move(x));
    };

    boost::thread server_thread(boost::bind(&DmpServer::run, boost::ref(server)));

    boost::asio::io_service io_service;
    boost::thread accept_thread(boost::bind(dmp::accept_loop, 1337, boost::ref(io_service), f));

    bool stop = false;
    while(!stop)
    {
        std::string input;
        std::getline(std::cin, input);

        std::cout << "Input: " << input << std::endl;

        if (input == "stop")
        {
            stop = true;
        }
        sleep(1);
    }

    io_service.stop();
    accept_thread.join();

    return 0;
}
