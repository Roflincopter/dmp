
#include "RTSPSender.hpp"

#include <dmp-library.hpp>
#include <connect.hpp>
#include <message.hpp>
#include <message_serializer.hpp>

#include <sstream>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {

    dmp::Connection conn = dmp::connect("localhost", 1337);

    message::Ping ping;
    std::cout << "Payload: " << ping.payload << std::endl;

    conn.send(ping);
    switch (conn.receive_type())
    {
        case message::Type::Ping :
        {
            message::Ping pi = conn.receive<message::Ping>();
            message::Pong po(pi);
            conn.send(po);
            break;
        }

        case message::Type::Pong :
        {
            message::Pong p = conn.receive<message::Pong>();
            std::cout << "Payload: " << p.payload << std::endl;
            break;
        }
    }


    if(argc < 2)
    {
        std::cout << "Please give the path to your music folder as first argument" << std::endl;
        return -1;
    }

    return 0;
}
