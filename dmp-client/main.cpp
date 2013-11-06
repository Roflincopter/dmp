
#include "RTSPSender.hpp"
#include "dmp_client.hpp"

#include "dmp-library.hpp"
#include "connect.hpp"
#include "message.hpp"
#include "message_serializer.hpp"
#include "dmp_client.hpp"

#include <boost/program_options.hpp>
#include <boost/thread.hpp>

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

    message::DmpCallbacks cbs;
    cbs.set(message::Type::Ping, std::function<void(message::Ping)>(std::bind(&DmpClient::handle_ping, &client, std::placeholders::_1))).
        set(message::Type::Pong, std::function<void(message::Ping)>(std::bind(&DmpClient::handle_pong, &client, std::placeholders::_1))).
        set(message::Type::NameRequest, std::function<void(message::NameRequest)>(std::bind(&DmpClient::handle_name_request, &client, std::placeholders::_1))).
        set(message::Type::SearchRequest, std::function<void(message::SearchRequest)>(std::bind(&DmpClient::handle_search_request, &client, std::placeholders::_1))).
        set(message::Type::SearchResponse, std::function<void(message::SearchResponse)>(std::bind(&DmpClient::handle_search_response, &client, std::placeholders::_1)));

    client.bind_callbacks(cbs);

    boost::thread(boost::bind(&DmpClient::run, &client));

    client.index("/home/dennis/Music");

    std::string input;
    do
    {
        std::getline(std::cin, input);

        std::stringstream ss(input);
        std::string cmd;
        ss >> cmd;
        ss >> std::ws;

        if (cmd == "index") {
            std::string arg;
            std::getline(ss, arg);
            client.index(arg);
        }
        if (cmd == "Search") {
            std::string arg;
            std::getline(ss, arg);
            client.search(arg);
        }

    } while(std::cin);

    return 0;
}
