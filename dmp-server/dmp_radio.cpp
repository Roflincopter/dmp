#include "dmp_radio.hpp"

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
, sink(gst_element_factory_make("tcpserversink", "send"))
, radio_mutex(new std::mutex)
, recv_port(port_pool->AllocateNumber())
, send_port(port_pool->AllocateNumber())
{
	if (!source || !buffer || !parser || !sink)
	{
		CHECK_GSTREAMER_COMPONENT(source);
		CHECK_GSTREAMER_COMPONENT(buffer);
		CHECK_GSTREAMER_COMPONENT(parser);
		CHECK_GSTREAMER_COMPONENT(sink);
		throw std::runtime_error("Could not create the pipeline components for this radio.");
	}

	g_object_set(G_OBJECT(source.get()), "port", gint(recv_port), nullptr);
	
	g_object_set(G_OBJECT(buffer.get()), "max-size-time", gint(100000000), nullptr);
	g_object_set(G_OBJECT(buffer.get()), "use-buffering", gboolean(true), nullptr);
	
	g_object_set(G_OBJECT(sink.get()), "port", gint(send_port), nullptr);
	
	gst_bin_add_many (GST_BIN(pipeline.get()), source.get(), buffer.get(), parser.get(), sink.get(), nullptr);
	if(!gst_element_link_many(source.get(), buffer.get(), parser.get(), sink.get(), nullptr)) {
		throw std::runtime_error("Linking the elements failed");
	}
}

void DmpRadio::listen()
{
	gst_element_set_state(pipeline.get(), GST_STATE_PLAYING);
}

//Reversing the logic here. The receiving end of the client is on the sender end of the radio and vice versa.
uint16_t DmpRadio::get_receiver_port()
{
	return send_port;
}

uint16_t DmpRadio::get_sender_port()
{
	return recv_port;
}

Playlist DmpRadio::get_playlist()
{
	return playlist;
}

void DmpRadio::stop()
{
	std::unique_lock<std::mutex> l(*radio_mutex);
	
	PlaylistEntry entry = playlist.front();
	auto sp = server.lock();
	sp->order_stop(entry.owner, name);
}

void DmpRadio::play()
{
	std::unique_lock<std::mutex> l(*radio_mutex);
	
	PlaylistEntry entry = playlist.front();
	auto sp = server.lock();
	sp->order_play(entry.owner, name);
}

void DmpRadio::pause()
{
	std::unique_lock<std::mutex> l(*radio_mutex);
	
	PlaylistEntry entry = playlist.front();
	auto sp = server.lock();
	sp->order_pause(entry.owner, name);
}

void DmpRadio::next()
{
	std::unique_lock<std::mutex> l(*radio_mutex);
	
	PlaylistEntry entry = playlist.front();
	auto sp = server.lock();
	sp->order_stop(entry.owner, name);
	
	playlist.erase(playlist.begin());
	entry = playlist.front();
	
	std::cout << "Ordering: " << entry.entry << std::endl;
	
	sp->order_stream(entry.owner, name, entry.entry, get_sender_port());
	
	sp->order_play(entry.owner, name);
}

void DmpRadio::eos_reached() {
	
	gst_element_set_state(pipeline.get(), GST_STATE_NULL);
	gst_element_set_state(pipeline.get(), GST_STATE_PLAYING);
	next();
}

void DmpRadio::queue(std::string queuer, std::string owner, dmp_library::LibraryEntry entry)
{
	std::unique_lock<std::mutex> l(*radio_mutex);
	
	bool queue_was_empty = playlist.empty();
	playlist.push_back({queuer, owner, entry});
	
	if (queue_was_empty) {
		PlaylistEntry entry = playlist.front();
		auto sp = server.lock();
		sp->order_stream(entry.owner, name, entry.entry, get_sender_port());
	}
}
