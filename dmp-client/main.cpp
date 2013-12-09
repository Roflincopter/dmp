
#include "dmp_client.hpp"

#include "dmp-library.hpp"
#include "message.hpp"
#include "message_serializer.hpp"
#include "dmp_client.hpp"
#include "dmp_client_gui.hpp"
#include "connect.hpp"

#include <boost/program_options.hpp>
#include <boost/thread.hpp>

#include <QApplication>

#include <sstream>
#include <iostream>
#include <string>

/* remove when finished with gstreamer testing */
#include "dmp_sender.hpp"
#include "dmp_receiver.hpp"

int main(int argc, char* argv[]) {

    using boost::program_options::value;
    boost::program_options::options_description desc;
    desc.add_options()
            ("help", "produce help message")
            ("server", value<std::string>(), "The server you want to connect to")
            ("port", value<uint16_t>(), "The port you want to connect to")
            ("name", value<std::string>(), "Your screen name")
            //("sender", value<bool>(), "Sending or not.")
            ;

    boost::program_options::positional_options_description pos_desc;
    pos_desc.add("server", 1).add("port", 1);

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(pos_desc).run(), vm);
    boost::program_options::notify(vm);
/*
    if (vm["sender"].as<bool>())
        DmpSender s("localhost", 2000, "test.mp3");
    else
        DmpReceiver r("localhost", 2001);
    return 0;
*/
    std::string name;
    if(!vm["name"].empty()) {
        name = vm["name"].as<std::string>();
    } else {
        name = boost::asio::ip::host_name();
    }

    int retval = 0;
    {
        int x = 0;
        QApplication qapp(x, nullptr);

        auto gui = std::make_shared<DmpClientGui>();
        auto client = std::make_shared<DmpClient>(name, dmp::connect(vm["server"].as<std::string>(), vm["port"].as<uint16_t>()));
        client->set_ui(gui);
        gui->set_client(client);

        gui->show();

        retval = qapp.exec();
    }
    return retval;
}
