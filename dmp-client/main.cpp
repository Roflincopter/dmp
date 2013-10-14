
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

    return 0;
}
