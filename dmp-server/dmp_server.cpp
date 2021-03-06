#include "dmp_server.hpp"
#include "dmp_radio.hpp"
#include "client_endpoint.hpp"
#include "message_callbacks.hpp"
#include "playlist.hpp"
#include "radio_state.hpp"
#include "accept.hpp"
#include "database.hpp"
#include "server_exceptions.hpp"

#include "key_management.hpp"
#include "container_manipulations.hpp"
#include "number_pool.hpp"

#include "dmp_config.hpp"

#include "debug_macros.hpp"

#include "user-odb.hpp"
#include "radio-odb.hpp"
#include "radio_admin-odb.hpp"

#include <odb/database.hxx>
#include <odb/result.hxx>
#include <odb/exception.hxx>
#include <odb/exceptions.hxx>
#include <odb/forward.hxx>

#include <sodium/crypto_pwhash_scryptsalsa208sha256.h>

#include <boost/asio/io_service.hpp>

#include <boost/filesystem/path.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/system/system_error.hpp>

#include <cstdint>
#include <exception>
#include <functional>
#include <iostream>
#include <stdexcept>


Authenticator::Authenticator(std::shared_ptr<odb::database> db)
: db(db)
{}

Authenticator::LoginResult Authenticator::login(std::string username, std::string password)
{
	odb::transaction t(db->begin());
	
	Authenticator::LoginResult lr;
	
	auto user = db->find<User>(username);
	if(!user) {
		lr = {false, "Username not found in database"};
	} else {
		std::string hashed_password(user->get_password());
		bool succes = !crypto_pwhash_scryptsalsa208sha256_str_verify(
			&hashed_password[0],
			password.data(),
			password.size()
		);

		if(succes) {
			lr = {true, ""};
		} else {
			lr = {false, "Wrong password"};
		}
	}
	
	t.commit();
	return lr;
}

Authenticator::RegisterResult Authenticator::register_username(std::string username, std::string password)
{
	odb::transaction t(db->begin());

	std::string hashed_password(crypto_pwhash_scryptsalsa208sha256_strbytes(), '\0');

	bool succes = !crypto_pwhash_scryptsalsa208sha256_str(
		&hashed_password[0],
		password.data(),
		password.size(),
		crypto_pwhash_scryptsalsa208sha256_opslimit_interactive(),
		crypto_pwhash_scryptsalsa208sha256_memlimit_interactive()
	);

	if(!succes) {
		throw std::runtime_error("Out of memory for password hashing");
	}

	User user(username, hashed_password);
	
	Authenticator::RegisterResult rr;
	
	try {
		t.tracer (odb::stderr_tracer);
		db->persist(user);
		t.commit();
		rr = {true, ""};
	} catch(odb::object_already_persistent const&) {
		rr = {false, "Username already exists in database"};
	} catch(...) {
		DEBUG_COUT << "You caught the wrong \"exception\"" << std::endl;
	}

	return rr;
}

DmpServer::DmpServer()
: server_io_service(std::make_shared<boost::asio::io_service>())
, connections()
, radios()
, port_pool(std::make_shared<NumberPool>(50000, 51000))
, db(initialize_database())
, auth(db)
{
	auto f = [this](Connection&& x){
		try {
			add_pending_connection(std::move(x));
		} catch(std::exception &e) {
			std::cerr << "Failed to initialize connection: " << e.what() << std::endl;
		}
	};

	accept_loop(1337, server_io_service, f);
}

void DmpServer::read_database()
{
	odb::transaction t(db->begin());
	
	try {
		odb::result<Radio> radios(db->query<Radio>());
		for(auto&& radio : radios) {
			add_radio(radio.get_name());
		}
	} catch(odb::exception& e) {
		DEBUG_COUT << e.what() << std::endl;
	}
}

void DmpServer::run()
{
	try {
		server_io_service->run();
	} catch (std::runtime_error& e) {
		DEBUG_COUT << e.what() << std::endl;
		run();
	}
}

void DmpServer::stop()
{
	server_io_service->stop();
}

void DmpServer::add_pending_connection(Connection&& c)
{
	std::shared_ptr<ClientEndpoint> cep = std::make_shared<ClientEndpoint>(
		std::move(c),
		server_io_service
	);
	
	pending_connections.push_back(cep);
	
	cep->get_callbacks().
		set(message::Type::LoginRequest, [this, cep](message::LoginRequest lr) {
			auto result = auth.login(lr.username, lr.password);
			if(!result.succes) {
				cep->forward(message::LoginResponse(result.succes, result.reason));
			} else {
				cep->set_name(lr.username);
				cep->get_callbacks().
					unset(message::Type::LoginRequest).
					unset(message::Type::RegisterRequest).
					unset(message::Type::Bye).
					unset(message::Type::PublicKey);
				add_permanent_connection(cep);
				remove_element(pending_connections, cep);
				cep->forward(message::LoginResponse(result.succes, ""));
			}
		}).
		set(message::Type::RegisterRequest, [this, cep](message::RegisterRequest r) {
			auto result = auth.register_username(r.username, r.password);
			cep->forward(message::RegisterResponse(result.succes, result.reason));
		}).
		set(message::Type::Bye, [this, cep](message::Bye b) {
			cep->handle_bye(b);
			server_io_service->post([this, cep](){
				remove_element(pending_connections, cep);
				cep->get_callbacks().clear();
			});
		}).
		set(message::Type::PublicKey, [this, cep](message::PublicKey r) {
			KeyPair pair = load_key_pair();
			
			//send key plain first then set up encryption as it is needed for signature verification.
			cep->forward(message::PublicKey(pair.public_key));
			
			cep->set_our_keys(
				pair.private_key,
				pair.public_key
			);
			cep->set_their_key(
				r.key
			);
		});
}

void DmpServer::add_permanent_connection(std::shared_ptr<ClientEndpoint> cep)
{
	auto username = cep->get_name();
	
	if(connections.find(username) != connections.end()) {
		connections[username]->terminate_connection();
	}

	server_io_service->post([this, cep, username]{
		cep->set_terminate_connection(std::bind(&DmpServer::remove_connection, this, username));

		std::weak_ptr<ClientEndpoint> wcep = cep;
		cep->get_callbacks().
			set(message::Type::SearchRequest, std::function<void(message::SearchRequest)>(std::bind(&DmpServer::handle_search, this, wcep, std::placeholders::_1))).
			set(message::Type::AddRadio, std::function<void(message::AddRadio)>(std::bind(&DmpServer::handle_add_radio, this, wcep, std::placeholders::_1))).
			set(message::Type::RemoveRadio, std::function<void(message::RemoveRadio)>(std::bind(&DmpServer::handle_remove_radio, this, wcep, std::placeholders::_1))).
			set(message::Type::RadioAction, std::function<void(message::RadioAction)>(std::bind(&DmpServer::handle_radio_action, this, std::placeholders::_1))).
			set(message::Type::SenderEvent, std::function<void(message::SenderEvent)>(std::bind(&DmpServer::handle_sender_event, this, std::placeholders::_1))).
			set(message::Type::TuneIn, std::function<void(message::TuneIn)>(std::bind(&DmpServer::handle_tune_in, this, wcep, std::placeholders::_1))).
			set(message::Type::Bye, std::function<void(message::Bye)>(std::bind(&ClientEndpoint::handle_bye, cep.get(), std::placeholders::_1))).
			set(message::Type::PlaylistUpdate, std::function<void(message::PlaylistUpdate)>(std::bind(&DmpServer::handle_playlist_update, this, std::placeholders::_1)));
		connections[username] = cep;

		std::map<std::string, Playlist> playlists;
		for(auto&& radio : radios) {
			playlists.emplace(radio.first, radio.second.get_playlist());
		}
		connections[username]->forward(message::Radios(playlists));

		std::map<std::string, RadioState> states;
		for(auto&& radio : radios) {
			states.emplace(radio.first, radio.second.get_state());
		}
		connections[username]->forward(message::RadioStates(message::RadioStates::Action::Set, states));
	});
}

void DmpServer::remove_connection(std::string name)
{
	server_io_service->post([this, name]{
		connections.erase(name);

		for(auto&& connection : connections) {
			std::cout << "Sending Disconnected to " << connection.first << std::endl;
			connection.second->forward(message::Disconnected(name));
		}

		for(auto&& radio : radios){
			radio.second.disconnect(name);
		}
	});
}

void DmpServer::handle_search(std::weak_ptr<ClientEndpoint> weak_origin, message::SearchRequest sr)
{
	auto origin = weak_origin.lock();
	if(!origin) {
		throw InvalidClientEndpoint();
	}
	std::function<void(message::SearchResponse)> cb = std::bind(&ClientEndpoint::forward<message::SearchResponse>, origin.get(), std::placeholders::_1);

	for(auto const& x : connections)
	{
		x.second->search(cb, sr);
	}
}

void DmpServer::add_radio(std::string radio_name) {
	auto radio_it = radios.emplace(
		std::make_pair(
			radio_name, 
			DmpRadio(
				radio_name, 
				shared_from_this(),
				port_pool,
				config::get_gst_folder_name().string()
			)
		)
	).first;
	
	radio_it->second.listen();
}

void DmpServer::remove_radio(std::string radio_name)
{
	auto radio_it = radios.find(radio_name);
	radios.erase(radio_it);
}

void DmpServer::gstreamer_debug(std::string reason)
{
	for(auto&& radio : radios) {
		std::cout << "created debug graph: " << radio.second.make_debug_graph(reason) << std::endl;
	}
}

void DmpServer::handle_add_radio(std::weak_ptr<ClientEndpoint> weak_origin, message::AddRadio ar)
{
	auto origin = weak_origin.lock();
	if(!origin) {
		throw InvalidClientEndpoint();
	}

	if(radios.find(ar.name) == radios.end()) {
		
		add_radio(ar.name);
		
		origin->forward(message::AddRadioResponse(ar.name, true, ""));
		
		for(auto connection : connections) {
			connection.second->forward(message::AddRadio(ar.name));
		}
		
		{
			odb::transaction t(db->begin());
		
			Radio r(ar.name);
			db->persist(r);
		
			SuperAdmin admin(db->find<User>(origin->get_name()), db->find<Radio>(ar.name));
			db->persist(admin);
		
			t.commit();
		}
	} else {
		origin->forward(message::AddRadioResponse(ar.name, false, "Radio with name " + ar.name + " already exists"));
	}
}

void DmpServer::handle_remove_radio(std::weak_ptr<ClientEndpoint> weak_origin, message::RemoveRadio rr) {
	auto origin = weak_origin.lock();
	if(!origin) {
		throw InvalidClientEndpoint();
	}

	if(radios.find(rr.name) != radios.end()) {
		{
			odb::transaction t(db->begin());
			
			auto user = db->find<User>(origin->get_name());
			auto radio = db->find<Radio>(rr.name);
			
			using query = odb::query<SuperAdmin>;
			auto result = db->query<SuperAdmin>(query::user == user->get_username()&& query::radio == radio->get_name());
			
			if(!result.empty()) {
				
				for(auto&& x : result) {
					db->erase(x);
				}
				
				using query = odb::query<Admin>;
				for(auto&& x : db->query<Admin>(query::radio == radio->get_name())) {
					db->erase(x);
				}
				
				db->erase(*radio);
				
				remove_radio(radio->get_name());
			} else {
				DEBUG_COUT << "Unrightfull delete attempted" << std::endl;
			}
			t.commit();
		}
		origin->forward(message::RemoveRadio(rr.name));
	}
}

void DmpServer::update_playlist(std::string radio_name, Playlist playlist)
{
	message::PlaylistUpdate::Action action(message::PlaylistUpdate::Action::Type::Update, {});
	for(auto& endpoint : connections) {
		endpoint.second->forward(message::PlaylistUpdate(action, radio_name, playlist));
	}
}

void DmpServer::handle_radio_action(message::RadioAction ra)
{
	auto& radio = radios.at(ra.radio_name);
	switch(ra.action)
	{
		case message::PlaybackAction::Next:
		{
			radio.next();
			break;
		}
		case message::PlaybackAction::Pause:
		{
			radio.pause();
			break;
		}
		case message::PlaybackAction::Play:
		{
			radio.play();
			break;
		}
		case message::PlaybackAction::Stop:
		{
			radio.stop();
			break;
		}
		//explicit falltrough.
		case message::PlaybackAction::NoAction:
		case message::PlaybackAction::Reset:
		default:
		{
			throw std::runtime_error("Unknown RadioAction was passed to the Server, action was: " + std::to_string(static_cast<message::Type_t>(ra.action)));
		}
	}
	
}

void DmpServer::handle_sender_event(message::SenderEvent se)
{
	auto& radio = radios.at(se.radio_name);
	radio.event_callbacks[se.event]();
}

void DmpServer::handle_tune_in(std::weak_ptr<ClientEndpoint> weak_origin, message::TuneIn ti)
{
	auto origin = weak_origin.lock();
	if(!origin) {
		throw InvalidClientEndpoint();
	}

	auto& radio = radios.at(ti.radio_name);

	if(ti.action == message::TuneIn::Action::TuneIn) {
		try {
			radio.add_listener(origin->get_name());
		} catch(std::runtime_error const& e) {
			std::cout << e.what() << std::endl;
		}
		origin->forward(message::ListenConnectionRequest(ti.radio_name, radio.get_receiver_port(origin->get_name())));
	} else if(ti.action == message::TuneIn::Action::TuneOff) {
		radio.remove_listener(origin->get_name());
	} else {
		throw std::runtime_error("Received a TuneIn message with unhandled action");
	}
}

void DmpServer::handle_playlist_update(message::PlaylistUpdate pu)
{
	auto& radio = radios.at(pu.radio_name);

	using Type = message::PlaylistUpdate::Action::Type;

	switch(pu.action.type) {
		case Type::NoAction: {
			break;
		}
		case Type::Append: {
			for(auto&& pe : pu.playlist) {
				pe.playlist_id = radio.queue(pe);
			}
			break;
		}
		case Type::Update: {
			break;
		}
		case Type::Insert: {
			break;
		}
		case Type::MoveUp: {
			radio.move_up(pu.action.ids);
			break;
		}
		case Type::MoveDown: {
			radio.move_down(pu.action.ids);
			break;
		}
		case Type::Remove: {
			radio.unqueue(pu.action.ids);
			break;
		}
		case Type::Reset: {
			break;
		}
		default: {
			throw std::runtime_error("Unknown playlist update action received.");
		}
	}
	
	for(auto& endpoint : connections) {
		endpoint.second->forward(pu);
	}
}

void DmpServer::order_stream(std::string client, std::string radio_name, uint32_t folder_id, dmp_library::LibraryEntry entry, uint16_t port)
{
	connections.at(client)->forward(message::StreamRequest(radio_name, folder_id, entry, port));
}

void DmpServer::update_radio_state()
{
	std::map<std::string, RadioState> states;
	for(auto&& radio : radios) {
		states.emplace(radio.first, radio.second.get_state());
	}
	
	for(auto& endpoint : connections) {
		endpoint.second->forward(message::RadioStates(message::RadioStates::Action::Set, states));
	}
}

void DmpServer::forward_receiver_action(std::string client, message::ReceiverAction ra)
{
	if(connections.find(client) != connections.end()) {
		connections.at(client)->forward(ra);
	}
}

void DmpServer::forward_sender_action(std::string client, message::SenderAction sa)
{
	if(connections.find(client) != connections.end()) {
		connections.at(client)->forward(sa);
	}
}

std::ostream&operator<<(std::ostream& os, const Authenticator::LoginResult& x) {
	return os << x.succes << " " << x.reason << std::endl;
}
