
#include "RTSPSender.hpp"
#include "dmp_client.hpp"

#include "dmp-library.hpp"
#include "connect.hpp"
#include "message.hpp"
#include "message_serializer.hpp"
#include "dmp_client.hpp"

#include <boost/program_options.hpp>

#include <sstream>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {

    using boost::program_options::value;
    boost::program_options::options_description desc;
    desc.add_options()
            ("help", "produce help message")
            ("server", value<std::string>(), "Server hostanme")
            ("port", value<uint16_t>(), "Destination port")
            ;

    boost::program_options::variables_map vm;
    boost::program_options::store(parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);


    dmp::Connection conn = dmp::connect(vm["server"].as<std::string>(), vm["port"].as<uint16_t>());

    DmpClient client(std::move(conn));
    //this also starts the asio eventloop

    return 0;
}
