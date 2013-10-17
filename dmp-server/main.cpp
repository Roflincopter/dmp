
#include "RTSPReceiver.hpp"

#include <dmp-library.hpp>
#include <accept.hpp>

#include <boost/thread.hpp>

#include <iostream>
#include <string>

int main(int argc, char** argv) {



    std::vector<dmp::connection> vec;

    std::function<void(dmp::connection&&)> f = [&](dmp::connection&& x){
        switch(x.receive_type())
        {
            case message::Type::Ping :
            {
                message::Ping pi = x.receive<message::Ping>();
                message::Pong po(pi);
                x.send(po);
                break;
            }
        }

        vec.emplace_back(std::move(x));
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

    for(auto& x : vec)
    {
        std::cout << "Wehew" << std::endl;
    }

    return 0;
}
