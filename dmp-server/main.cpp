
#include "RTSPReceiver.hpp"

#include <dmp-library.hpp>
#include <accept.hpp>

#include <boost/thread.hpp>

#include <iostream>
#include <string>

int main(int argc, char** argv) {

    //dmp::connection connection = dmp::accept(1337);

    boost::asio::io_service io_service;
    boost::thread t(boost::bind(dmp::accept_loop, 1337, boost::ref(io_service)));

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
    std::cout << "Joining Thread" << std::endl;
    t.join();
    std::cout << "Joined Thread" << std::endl;

    /*
    dmp_library::Library music = connection.receive<dmp_library::Library>();

    for(auto const& e : music)
    {
        std::cout << e << std::endl;
    }
    */
    //RTSPReceiver receiver("127.0.0.1", 6666, 6667, 320);
    //receiver.start();
    return 0;
}
