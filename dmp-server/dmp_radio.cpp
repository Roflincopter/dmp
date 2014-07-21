#include "dmp_radio.hpp"

#include "debug_macros.hpp"
#include "message_outputter.hpp"

#include <string>
#include <tuple>

DmpRadio::DmpRadio(std::string name, std::weak_ptr<DmpServerInterface> server, std::shared_ptr<NumberPool> port_pool)
: GStreamerBase("tcp_bridge")
, name(name)
, server(server)
, port_pool(port_pool)
, source(gst_element_factory_make("tcpserversrc", "recv"))
, buffer(gst_element_factory_make("queue2", "buffer"))
, parser(gst_element_factory_make("mpegaudioparse", "parser"))
, tee(gst_element_factory_make("tee", "tee"))
//, sink(gst_element_factory_make("tcpserversink", "send"))
, tee_src_pad_template(gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS(tee.get()), "src_%u"))
, radio_mutex(new std::mutex)
, recv_port(port_pool->allocate_number())
{
	if (!source || !buffer || !parser || !tee)
	{
		CHECK_GSTREAMER_COMPONENT(source);
		CHECK_GSTREAMER_COMPONENT(buffer);
		CHECK_GSTREAMER_COMPONENT(parser);
		CHECK_GSTREAMER_COMPONENT(tee);
		throw std::runtime_error("Could not create the pipeline components for this radio.");
	}
	
	if(!tee_src_pad_template) {
		throw std::runtime_error("Unable to get pad template for radio Tee.");
	}

	g_object_set(G_OBJECT(source.get()), "host", "0.0.0.0", nullptr);
	g_object_set(G_OBJECT(source.get()), "port", gint(recv_port), nullptr);
	
	g_object_set(G_OBJECT(buffer.get()), "max-size-time", gint(100000000), nullptr);
	g_object_set(G_OBJECT(buffer.get()), "use-buffering", gboolean(true), nullptr);
	
	gst_bin_add_many (GST_BIN(pipeline.get()), source.get(), buffer.get(), parser.get(), tee.get(), nullptr);
	if(!gst_element_link_many(source.get(), buffer.get(), parser.get(), tee.get(), nullptr)) {
		throw std::runtime_error("Linking the elements failed");
	}
}

void DmpRadio::listen()
{
	gst_element_set_state(pipeline.get(), GST_STATE_PLAYING);
}

void DmpRadio::add_listener(std::string name)
{
	if(branches.find(name) != branches.end()) {
		throw std::runtime_error(name + "was already listening to radio: " + this->name);
	}
	
	DmpRadioEndpoint listener(name, port_pool->allocate_number());
	DEBUG_COUT << listener.get_sink() << std::endl;
	
	GstPad* tee_pad = gst_element_request_pad(tee.get(), tee_src_pad_template.get(), nullptr, nullptr);
	DEBUG_COUT << listener.get_sink() << std::endl;
	
	if(!tee_pad) {
		throw std::runtime_error("failed to create a request pad from the tee");
	}
	
	auto branch_it = branches.emplace(
		name, 
		TeeBranch(gst_pad_get_name(tee_pad), std::unique_ptr<GstPad, GStreamerRequestPadDeleter>(tee_pad, {tee.get()}), std::move(listener))
	).first;
	DEBUG_COUT << branch_it->second.endpoint.get_sink() << std::endl;
	
	branch_it->second.endpoint.play();
	DEBUG_COUT << branch_it->second.endpoint.get_sink() << std::endl;
	
	gst_bin_add(GST_BIN(pipeline.get()), branch_it->second.endpoint.get_bin());
	gst_pad_link(branch_it->second.pad.get(), branch_it->second.endpoint.get_sink_pad());
	
}

//Reversing the logic here. The receiving end of the client is on the sender end of the radio and vice versa.
uint16_t DmpRadio::get_sender_port()
{
	return recv_port;
}

uint16_t DmpRadio::get_receiver_port(std::string name)
{
	return branches.at(name).endpoint.get_port();
}

Playlist DmpRadio::get_playlist()
{
	return playlist;
}

void DmpRadio::stop()
{
	std::unique_lock<std::mutex> l(*radio_mutex);
	
	if(playlist.empty()) {
		return;
	}
	
	PlaylistEntry entry = playlist.front();
	
	auto sp = server.lock();
	DEBUG_COUT << "Ordering stop from: " << entry.owner << " of radio: " << name << std::endl;
	
	for (auto&& branch : branches)
	{
		sp->forward_receiver_action(branch.first, message::ReceiverAction(name, message::PlaybackAction::Stop));
	}
	/*
	gst_element_set_state(pipeline.get(), GST_STATE_READY);
	
	{
		GstState state;
		GstState pending;
		//force state change to happen.
		gst_element_get_state(pipeline.get(), &state, &pending, GST_CLOCK_TIME_NONE);
	}
	*/
	sp->forward_sender_action(entry.owner, message::SenderAction(name, message::PlaybackAction::Stop));

	stopped = true;
}

void DmpRadio:: play()
{
	std::cout << "Play called on radio: " << name << std::endl;
	std::unique_lock<std::mutex> l(*radio_mutex);
	
	if (playlist.empty()) {
		return;
	}
	
	PlaylistEntry entry = playlist.front();
	auto sp = server.lock();
	DEBUG_COUT << "Ordering play from: " << entry.owner << " of radio: " << name << std::endl;
	DEBUG_COUT << "Current playlist: " << playlist;
	
	sp->order_stream(entry.owner, name, entry.entry, get_sender_port());
	
	gst_element_set_state(pipeline.get(), GST_STATE_PLAYING);
	
	for(auto&& branch : branches)
	{
		DEBUG_COUT << "Sending listens" << std::endl;
		sp->forward_receiver_action(branch.first, message::ReceiverAction(name, message::PlaybackAction::Play));
	}
	
	sp->forward_sender_action(entry.owner, message::SenderAction(name, message::PlaybackAction::Play));
	
	make_debug_graph();
	
	stopped = false;
}

void DmpRadio::pause()
{
	std::cout << "Paused called on radio: " << name << std::endl;
	gst_element_set_state(pipeline.get(), GST_STATE_PAUSED);
	std::unique_lock<std::mutex> l(*radio_mutex);
	
	auto sp = server.lock();
	PlaylistEntry entry = playlist.front();
	
	sp->forward_sender_action(entry.owner, message::SenderAction(name, message::PlaybackAction::Pause));
	for(auto&& branch : branches)
	{
		sp->forward_receiver_action(branch.first, message::ReceiverAction(name, message::PlaybackAction::Pause));
	}
	
	GstState state;
	GstState pending;
	gst_element_get_state(pipeline.get(), &state, &pending, GST_CLOCK_TIME_NONE);
}

void DmpRadio::next()
{
	DEBUG_COUT << "before: " << playlist << std::endl;
	
	std::unique_lock<std::mutex> l(*radio_mutex);
	
	
	
	if(playlist.empty()) {
		DEBUG_COUT << "Playlist after next was empty returning" << std::endl;
		return;
	}
	
	auto sp = server.lock();
	
	if(!stopped) {
		sp->forward_sender_action(playlist.front().owner, message::SenderAction(name, message::PlaybackAction::Reset));
		gst_element_set_state(pipeline.get(), GST_STATE_READY);
		
		{
			GstState state;
			GstState pending;
			gst_element_get_state(pipeline.get(), &state, &pending, GST_CLOCK_TIME_NONE);
		}
	}
	
	playlist.erase(playlist.begin());
	
	DEBUG_COUT << "Stopped was: " << std::boolalpha << stopped << std::endl;
	DEBUG_COUT << "after: " << playlist << std::endl;
	
	if(!stopped) { 
		sp->update_playlist(name, playlist);
		l.unlock();
		play();
	}

	
}

void DmpRadio::eos_reached()
{
	gst_element_set_state(pipeline.get(), GST_STATE_NULL);
	DEBUG_COUT << "dmp_radio: EOS reached" << std::endl;
}

void DmpRadio::queue(std::string queuer, std::string owner, dmp_library::LibraryEntry entry)
{
	std::unique_lock<std::mutex> l(*radio_mutex);
	
	playlist.push_back({queuer, owner, entry});
}
