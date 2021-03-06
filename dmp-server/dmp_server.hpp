#pragma once

#include "dmp_server_interface.hpp"
#include "dmp_radio.hpp"

#include "message.hpp"

#include "playlist.hpp"

#include <thread>
#include <stdint.h>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class ClientEndpoint;
class Connection;
class NumberPool;

namespace boost { namespace asio { class io_service; } }

namespace dmp_library { struct LibraryEntry; }

namespace odb { class database; }

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
	std::map<std::string, DmpRadio> radios;
	std::shared_ptr<NumberPool> port_pool;
	std::shared_ptr<odb::database> db;
	Authenticator auth;

public:
	DmpServer();
	~DmpServer() = default;

	void run();
	void stop();
	
	void read_database();
	
	void add_pending_connection(Connection&& c);
	void add_permanent_connection(std::shared_ptr<ClientEndpoint> c);
	void remove_connection(std::string name);
	void add_radio(std::string);
	void remove_radio(std::string);
	
	void gstreamer_debug(std::string reason);

	void handle_search(std::weak_ptr<ClientEndpoint> weak_origin, message::SearchRequest sr);
	void handle_add_radio(std::weak_ptr<ClientEndpoint> weak_origin, message::AddRadio ar);
	void handle_remove_radio(std::weak_ptr<ClientEndpoint> weak_origin, message::RemoveRadio rr);
	void handle_radio_action(message::RadioAction ra);
	void handle_sender_event(message::SenderEvent se);
	void handle_tune_in(std::weak_ptr<ClientEndpoint> weak_origin, message::TuneIn ti);
	void handle_playlist_update(message::PlaylistUpdate pu);

	void update_playlist(std::string radio_name, Playlist playlist) override final;
	
	virtual void forward_receiver_action(std::string client, message::ReceiverAction ra) override final;
	virtual void forward_sender_action(std::string client, message::SenderAction sa) override final;
	virtual void order_stream(std::string client, std::string radio_name, uint32_t folder_id, dmp_library::LibraryEntry entry, uint16_t port) override final;
	virtual void update_radio_state() override final;
};

std::ostream& operator<< (std::ostream& os, Authenticator::LoginResult const& x);
