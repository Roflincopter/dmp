
#include "RTSPReceiver.hpp"

#include <dmp-library.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/asio.hpp>

#include <sstream>

int main(int argc, char** argv) {

    using namespace boost::asio::ip;

    boost::asio::io_service io_service;
    tcp::socket sock(io_service);
    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 1337));

    acceptor.accept(sock);
    boost::system::error_code e;

    boost::asio::streambuf buff;
    try {
        boost::asio::read(sock, buff, boost::asio::transfer_all());  // for example
    }
    catch (boost::system::system_error &e){
        if (e.code() != boost::asio::error::misc_errors::eof)
        {
            std::cerr << e.code().category().name() << std::endl;
            exit(1);
        }
    }

    std::istream iss(&buff);
    boost::archive::text_iarchive iar(iss);

    dmp_library::Library music;
    iar & music;

    for(auto const& e : music)
    {
        std::cout << e << std::endl;
    }

    //RTSPReceiver receiver("127.0.0.1", 6666, 6667, 320);
    //receiver.start();
    return 0;
}
