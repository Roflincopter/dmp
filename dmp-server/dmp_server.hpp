#pragma once

#include "dmp_radio.hpp"
#include "client_endpoint.hpp"
#include "dmp_server_interface.hpp"

#include "connection.hpp"
#include "message_callbacks.hpp"
#include "number_pool.hpp"
#include "container_manipulations.hpp"

#include "user-odb.hpp"

#include <thread>

class Authenticator
{
	std::shared_ptr<odb::database> db;

public:
	struct LoginResult {
		bool succes;
		std::string reason;
	};
	
	struct RegisterResult {
		bool succes;
		std::string reason;
	};
	
	Authenticator(std::shared_ptr<odb::database> db);

	LoginResult login(std::string username, std::string password);
	
	RegisterResult register_username(std::string username, std::string password);
};

class DmpServer : public DmpServerInterface, public std::enable_shared_from_this<DmpServerInterface>
{
	std::shared_ptr<boost::asio::io_service> server_io_service;
	std::vector<std::shared_ptr<ClientEndpoint>> pending_connections;
	std::map<std::string, std::shared_ptr<ClientEndpoint>> connections;
	std::map<std::string, std::pair<std::thread, DmpRadio>> radios;
	std::shared_ptr<NumberPool> port_pool;
	std::shared_ptr<odb::database> db;
	Authenticator auth;

public:
	DmpServer();
	~DmpServer();

	void run();
	void stop();
	
	void read_database();
	
	void add_pending_connection(Connection&& c);
	void add_permanent_connection(std::shared_ptr<ClientEndpoint> c);
	void remove_connection(std::string name);
	void add_radio(std::string);
	void remove_radio(std::string);

	bool handle_search(std::weak_ptr<ClientEndpoint> weak_origin, message::SearchRequest sr);
	bool handle_add_radio(std::weak_ptr<ClientEndpoint> weak_origin, message::AddRadio ar);
	bool handle_remove_radio(std::weak_ptr<ClientEndpoint> weak_origin, message::RemoveRadio rr);
	bool handle_queue(message::Queue queue);
	bool handle_radio_action(message::RadioAction ra);
	bool handle_sender_event(message::SenderEvent se);
	bool handle_tune_in(std::weak_ptr<ClientEndpoint> weak_origin, message::TuneIn ti);
	bool handle_unqueue(message::UnQueue uq);

	void update_playlist(std::string radio_name, Playlist playlist) override final;
	
	virtual void forward_receiver_action(std::string client, message::ReceiverAction ra) override final;
	virtual void forward_sender_action(std::string client, message::SenderAction sa) override final;
	virtual void order_stream(std::string client, std::string radio_name, uint32_t folder_id, dmp_library::LibraryEntry entry, uint16_t port) override final;
};
