#include "dmp_server.hpp"
#include "message_callbacks.hpp"
#include "timed_debug.hpp"
#include "playlist.hpp"
#include "radio_state.hpp"
#include "accept.hpp"
#include "database.hpp"

#include "user-odb.hpp"
#include "radio-odb.hpp"
#include "radio_admin-odb.hpp"
#include <odb/core.hxx>
#include <odb/database.hxx>
#include <odb/result.hxx>
#include <odb/query.hxx>

#include <boost/thread.hpp>

#include "fusion_outputter.hpp"

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
	} else if(password == user->get_password()) {
		lr = {true, ""};
	} else {
		lr = {false, "Wrong password"};
	}
	
	t.commit();
	return lr;
}

Authenticator::RegisterResult Authenticator::register_username(std::string username, std::string password)
{
	odb::transaction t(db->begin());
	User user(username, password);
	
	Authenticator::RegisterResult rr;
	
	try {
		t.tracer (odb::stderr_tracer);
		db->persist(user);
		t.commit();
		rr = {true, ""};
	} catch(odb::object_already_persistent const& e) {
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
	gst_init(0, nullptr);
	
	auto f = [this](Connection&& x){
		try {
			add_pending_connection(std::move(x));
		} catch(std::exception &e) {
			std::cerr << "Failed to initialize connection: " << e.what() << std::endl;
		}
	};

	accept_loop(1337, server_io_service, f);
}

DmpServer::~DmpServer()
{
	for(auto&& radio : radios) {
		radio.second.second.stop_loop();
		radio.second.first.join();
	}
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
	
	cep->set_terminate_connection([this, cep](){
		cep->cancel_pending_asio();
		remove_element(pending_connections, cep);
	});
	
	pending_connections.push_back(cep);
	
	cep->get_callbacks().
		set(message::Type::LoginRequest, [this, cep](message::LoginRequest lr) {
			auto result = auth.login(lr.username, lr.password);
			if(!result.succes) {
				cep->forward(message::LoginResponse(result.succes, result.reason));
				return;
			} else {
				cep->set_name(lr.username);
				cep->get_callbacks().
					unset(message::Type::LoginRequest).
					unset(message::Type::RegisterRequest);
				add_permanent_connection(cep);
				remove_element(pending_connections, cep);
				cep->forward(message::LoginResponse(result.succes, ""));
			}
		}).
		set(message::Type::RegisterRequest, [this, cep](message::RegisterRequest r) {
			auto result = auth.register_username(r.username, r.password);
			cep->forward(message::RegisterResponse(result.succes, result.reason));
		});
}

void DmpServer::add_permanent_connection(std::shared_ptr<ClientEndpoint> cep)
{
	auto username = cep->get_name();
	
	cep->set_terminate_connection(std::bind(&DmpServer::remove_connection, this, username));

	cep->get_callbacks().
		set(message::Type::SearchRequest, std::function<void(message::SearchRequest)>(std::bind(&DmpServer::handle_search, this, cep, std::placeholders::_1))).
		set(message::Type::AddRadio, std::function<void(message::AddRadio)>(std::bind(&DmpServer::handle_add_radio, this, cep, std::placeholders::_1))).
		set(message::Type::RemoveRadio, std::function<void(message::RemoveRadio)>(std::bind(&DmpServer::handle_remove_radio, this, cep, std::placeholders::_1))).
		set(message::Type::Queue, std::function<void(message::Queue)>(std::bind(&DmpServer::handle_queue, this, std::placeholders::_1))).
		set(message::Type::RadioAction, std::function<void(message::RadioAction)>(std::bind(&DmpServer::handle_radio_action, this, std::placeholders::_1))).
		set(message::Type::SenderEvent, std::function<void(message::SenderEvent)>(std::bind(&DmpServer::handle_sender_event, this, std::placeholders::_1))).
		set(message::Type::TuneIn, std::function<void(message::TuneIn)>(std::bind(&DmpServer::handle_tune_in, this, cep, std::placeholders::_1)));
	connections[username] = cep;

	std::map<std::string, Playlist> playlists;
	for(auto&& radio : radios) {
		playlists.emplace(radio.first, radio.second.second.get_playlist());
	}
	connections[username]->forward(message::Radios(playlists));

	std::map<std::string, RadioState> states;
	for(auto&& radio : radios) {
		states.emplace(radio.first, std::move(radio.second.second.get_state()));
	}
	connections[username]->forward(message::RadioStates(message::RadioStates::Action::Set, states));
}

void DmpServer::remove_connection(std::string name)
{
	connections.at(name)->cancel_pending_asio();
	connections.erase(name);
	for(auto&& radio : radios)
	{
		radio.second.second.disconnect(name);
	}
}

void DmpServer::handle_search(std::shared_ptr<ClientEndpoint> origin, message::SearchRequest sr)
{
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
			std::make_pair(
				std::thread(), 
				DmpRadio(
					radio_name, 
					shared_from_this(),
					port_pool
				)
			)
		)
	).first;
	
	radio_it->second.first = std::thread(std::bind(&DmpRadio::run_loop, std::ref(radio_it->second.second)));
	
	radio_it->second.second.listen();
}

void DmpServer::remove_radio(std::string radio_name)
{
	auto radio_it = radios.find(radio_name);
	radio_it->second.second.stop_loop();
	radio_it->second.first.join();
	radios.erase(radio_it);
}

void DmpServer::handle_add_radio(std::shared_ptr<ClientEndpoint> origin, message::AddRadio ar)
{
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

void DmpServer::handle_remove_radio(std::shared_ptr<ClientEndpoint> origin, message::RemoveRadio rr) {
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

void DmpServer::handle_queue(message::Queue queue)
{
	radios.at(queue.radio).second.queue(queue.queuer, queue.owner, queue.entry);
	message::PlaylistUpdate::Action action(message::PlaylistUpdate::Action::Type::Update, 0, 0);
	for(auto& endpoint : connections) {
		endpoint.second->forward(message::PlaylistUpdate(action, queue.radio, radios.at(queue.radio).second.get_playlist()));
	}
}

void DmpServer::update_playlist(std::string radio_name, Playlist playlist)
{
	message::PlaylistUpdate::Action action(message::PlaylistUpdate::Action::Type::Update, 0, 0);
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
			radio.second.next();
			break;
		}
		case message::PlaybackAction::Pause:
		{
			radio.second.pause();
			break;
		}
		case message::PlaybackAction::Play:
		{
			radio.second.play();
			break;
		}
		case message::PlaybackAction::Stop:
		{
			radio.second.stop();
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
	radio.second.event_callbacks[se.event]();
}

void DmpServer::handle_tune_in(std::shared_ptr<ClientEndpoint> origin, message::TuneIn ti)
{
	auto& radio = radios.at(ti.radio_name);

	if(ti.action == message::TuneIn::Action::TuneIn) {
		try {
			radio.second.add_listener(origin->get_name());
		} catch(std::runtime_error const& e) {
			std::cout << e.what() << std::endl;
			return;
		}
		origin->forward(message::ListenConnectionRequest(ti.radio_name, radio.second.get_receiver_port(origin->get_name())));
	} else if(ti.action == message::TuneIn::Action::TuneOff) {
		radio.second.remove_listener(origin->get_name());
	} else {
		throw std::runtime_error("Received a TuneIn message with unhandled action");
	}
}

void DmpServer::order_stream(std::string client, std::string radio_name, dmp_library::LibraryEntry entry, uint16_t port)
{
	connections.at(client)->forward(message::StreamRequest(radio_name, entry, port));
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

