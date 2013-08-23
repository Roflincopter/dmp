
#include "RTSPSender.hpp"

#include <dmp-library.hpp>

#include <boost/asio.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>


#include <sstream>
#include <string>

int main(int argc, char* argv[]) {

    using namespace boost::asio::ip;

    if(argc < 2)
    {
        std::cout << "Please give the path to your music folder as first argument" << std::endl;
        return -1;
    }

    std::string music_path = std::string(argv[1]);music_path = std::string(argv[1]);
    dmp_library::Library music = dmp_library::create_library(music_path);

    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query("127.0.0.1", "1337");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;

    tcp::socket socket(io_service);

    socket.connect(*endpoint_iterator);

    std::ostringstream oss;
    boost::archive::text_oarchive oar(oss);
    oar & music;

    std::string data = oss.str();
    boost::asio::write(socket, boost::asio::buffer(data));
    socket.close();

    //RTSPSender sender("127.0.0.1", 6666, 6667, 320);
    //sender.start("test.mp3");
    return 0;
}
