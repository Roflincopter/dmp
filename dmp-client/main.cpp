
#include "RTSPSender.hpp"

#include <dmp-library.hpp>
#include <connect.hpp>
#include <message.hpp>
#include <message_serializer.hpp>

#include <sstream>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::stringstream ss;
    {
        //std::ofstream ofs("/tmp/temp");
        boost::archive::text_oarchive oar(ss);
        message::Ping p;
        message::serialize(oar, p);
        std::cout << ss.str().size() << std::endl;
        std::cout << ss.str() << std::endl;
    }
    message::Ping pi;
    {
        boost::archive::text_iarchive iar(ss);
        message::serialize(iar, pi);
    }

    {
        std::stringstream ss;
        boost::archive::text_oarchive oar(ss);
        message::Pong po(pi);
        message::serialize(oar, po);
        std::cout << ss.str() << std::endl;
    }
    if(argc < 2)
    {
        std::cout << "Please give the path to your music folder as first argument" << std::endl;
        return -1;
    }

    return 0;
}
