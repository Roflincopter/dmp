
#include "RTSPSender.hpp"

#include <dmp-library.hpp>
#include <connect.hpp>

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if(argc < 2)
    {
        std::cout << "Please give the path to your music folder as first argument" << std::endl;
        return -1;
    }

    std::string music_path = std::string(argv[1]);music_path = std::string(argv[1]);
    dmp_library::Library music = dmp_library::create_library(music_path);

    {
        dmp::connection connection = dmp::connect("localhost", 1337);
        connection.send(music);
    } //destruction closes the socket.

    //RTSPSender sender("127.0.0.1", 6666, 6667, 320);
    //sender.start("test.mp3");
    return 0;
}
