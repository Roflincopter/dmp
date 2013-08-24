
#include "RTSPReceiver.hpp"

#include <dmp-library.hpp>
#include <accept.hpp>

#include <iostream>
#include <string>

int main(int argc, char** argv) {

    dmp::connection connection = dmp::accept(1337);
    dmp_library::Library music = connection.receive<dmp_library::Library>();

    for(auto const& e : music)
    {
        std::cout << e << std::endl;
    }
    //RTSPReceiver receiver("127.0.0.1", 6666, 6667, 320);
    //receiver.start();
    return 0;
}
