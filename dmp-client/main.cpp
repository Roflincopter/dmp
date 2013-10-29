
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
            ("server", value<std::string>(), "The server you want to connect to")
            ("port", value<uint16_t>(), "The port you want to connect to")
            ("name", value<std::string>(), "Your screen name");

    boost::program_options::positional_options_description pos_desc;
    pos_desc.add("server", 1).add("port", 1);

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(pos_desc).run(), vm);
    boost::program_options::notify(vm);

    dmp::Connection conn = dmp::connect(vm["server"].as<std::string>(), vm["port"].as<uint16_t>());

    std::string name;
    if(!vm["name"].empty()) {
        name = vm["name"].as<std::string>();
    } else {
        name = boost::asio::ip::host_name();
    }

    DmpClient client(name, std::move(conn));
    //this also starts the asio eventloop

    return 0;
}
