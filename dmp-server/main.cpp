
#include "RTSPReceiver.hpp"

#include "dmp-library.hpp"
#include "accept.hpp"

#include <boost/thread.hpp>

#include <iostream>
#include <string>

int main(int, char**) {

    std::vector<dmp::Connection> vec;

    std::function<void(dmp::Connection&&)> f = [&](dmp::Connection&& x){
        vec.emplace_back(std::move(x));
        message::Ping pi;
        vec.back().send(pi);

        sleep(3);

        pi = message::Ping();
        vec.back().send(pi);
    };

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

    std::cout << "Wehew" << vec.size() << std::endl;

    return 0;
}
