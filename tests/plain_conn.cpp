
#include "accept.hpp"
#include "connect.hpp"
#include "connection.hpp"
#include "message.hpp"
#include "message_callbacks.hpp"
#include "message_switch.hpp"

#include <boost/asio/io_service.hpp>

#include <memory>
#include <thread>

struct Client {
	Connection connection;
	message::DmpCallbacks callbacks;
	MessageSwitch messageswitch;
	
private:
	int pong_received_count = 0;
public:
	
	Client(Connection&& conn)
	: connection(std::move(conn)) 
	, callbacks(std::bind(&Client::receive, this), {})
	, messageswitch(make_message_switch()) {
		callbacks
			.set(message::Type::Ping, [this](message::Ping p){
				connection.send(message::Pong(p.payload));
				return true;
			})
			.set(message::Type::Pong, [this](message::Pong){
				++pong_received_count;
				if(pong_received_count < 5) {
					connection.send(message::Ping());
					return true;
				} else {
					return false;
				}
			});
	}
	
	template <typename T>
	void forward(T x) {
		connection.send(x);
	}
	
	void handle_type(message::Type t) {
		messageswitch.handle_message(connection, t, callbacks);
	}
	
	void receive() {
		connection.async_receive_type(std::bind(&Client::handle_type, this, std::placeholders::_1));
	}
};

struct Server {
	std::vector<std::shared_ptr<Client>> conns;
	
	Server()
	: conns() {}
	
	void add_connection(std::shared_ptr<Client> client) {
		conns.emplace_back(client);
		
		client->receive();
	}
};

int main() {

	auto server_io = std::make_shared<boost::asio::io_service>();
	auto client_io1 = std::make_shared<boost::asio::io_service>();
	auto client_io2 = std::make_shared<boost::asio::io_service>();
	
	Server server;
	
	accept_loop(2000, server_io, [&server](Connection&& c) {
		auto c_ptr = std::make_shared<Client>(std::move(c));
		server.add_connection(c_ptr);
	});
	
	Client client1(connect("127.0.0.1", 2000, client_io1));
	client1.receive();
	
	Client client2(connect("127.0.0.1", 2000, client_io2));
	client2.receive();
	
	std::thread t1;
	std::thread t2;
	std::thread t3;
	
	try {
		t1 = std::thread([server_io]{
			server_io->run();
		});
			
		t2 = std::thread([client_io1]{
			client_io1->run();
		});
		
		t3 = std::thread([client_io2]{
			client_io2->run();
		});
	} catch(std::exception& ex) {
		std::cout << ex.what() << std::endl;
		return -1;
	}
	
	client1.forward(message::Ping());
	client2.forward(message::Ping());
	
	t2.join();
	t3.join();
	server_io->stop();
	t1.join();
	
	return 0;
}
