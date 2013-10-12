
#include "RTSPSender.hpp"

#include <dmp-library.hpp>
#include <connect.hpp>
#include <message.hpp>
#include <message_serializer.hpp>

#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    {
        std::ofstream ofs("/tmp/temp");
        boost::archive::text_oarchive oar(ofs);
        message::Ping p;
        message::serialize(oar, p);
    }
    message::Ping pi;
    {
        std::ifstream ifs("/tmp/temp");
        boost::archive::text_iarchive iar(ifs);
        message::serialize(iar, pi);
    }

    {
        std::ofstream ofs("/tmp/temp");
        boost::archive::text_oarchive oar(ofs);
        message::Pong po(pi);
        message::serialize(oar, po);
    }
    if(argc < 2)
    {
        std::cout << "Please give the path to your music folder as first argument" << std::endl;
        return -1;
    }

    //std::string music_path = std::string(argv[1]);music_path = std::string(argv[1]);
    //dmp_library::Library music = dmp_library::create_library(music_path);

    {
        //std::unique_ptr<dmp::message> resmess = reqmess->respond();

        //connection.send(resmess.get());

        //connection.send(music);
    } //destruction closes the socket.

    //RTSPSender sender("127.0.0.1", 6666, 6667, 320);
    //sender.start("test.mp3");
    return 0;
}
