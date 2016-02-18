#include "dmp_radio.hpp"

#include "dmp_server_interface.hpp"

#include "number_pool.hpp"

#include <glib/gtypes.h>

#include <gobject/gobject.h>

#include <gst/gstbin.h>
#include <gst/gstelement.h>
#include <gst/gstelementfactory.h>
#include <gst/gstghostpad.h>
#include <gst/gstobject.h>
#include <gst/gstpad.h>
#include <gst/gstutils.h>

#include <stddef.h>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <string>

DmpRadio::DmpRadio(std::string name, std::weak_ptr<DmpServerInterface> server, std::shared_ptr<NumberPool> port_pool, std::string gst_dir)
: GStreamerBase("tcp_bridge", gst_dir)
, name(name)
, server(server)
, port_pool(port_pool)
, source(gst_element_factory_make("tcpserversrc", "recv"))
, buffer(gst_element_factory_make("queue2", "buffer"))
, parser(gst_element_factory_make("mpegaudioparse", "parser"))
, tee(gst_element_factory_make("tee", "tee"))
, tee_src_pad_template(gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS(tee.get()), "src_%u"))
, fake_pad(gst_element_request_pad(tee.get(), tee_src_pad_template.get(), nullptr, nullptr), GStreamerRequestPadDeleter(tee.get()))
, fake_buffer(gst_element_factory_make("queue2", "fake_buffer"))
, fake_sink(gst_element_factory_make("fakesink", "fakesink"))
, radio_mutex(new std::recursive_mutex)
, playlist_id(0)
, recv_port(port_pool->allocate_number())
, event_callbacks()
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
	
	g_object_set(G_OBJECT(buffer.get()), "max-size-time", gint(30000000000), nullptr);
	g_object_set(G_OBJECT(buffer.get()), "use-buffering", gboolean(true), nullptr);

	g_object_set(G_OBJECT(fake_sink.get()), "sync", gboolean(true), nullptr);

	gst_bin_add_many (GST_BIN(pipeline.get()), source.get(), buffer.get(), parser.get(), tee.get(), fake_buffer.get(), fake_sink.get(), nullptr);

	if(!gst_element_add_pad(fake_buffer.get(), gst_ghost_pad_new("tee_fake_sink", gst_element_get_static_pad(fake_buffer.get(), "sink"))))
	{
		throw std::runtime_error("Adding ghost pad failed");
	}

	if(!gst_element_link_many(source.get(), buffer.get(), parser.get(), tee.get(), nullptr)) {
		throw std::runtime_error("Linking the elements failed");
	}

	if(!gst_element_link_many(fake_buffer.get(), fake_sink.get(), nullptr)) {
		throw std::runtime_error("Linking the fake elements failed");
	}

	if(gst_pad_link(fake_pad.get(), gst_element_get_static_pad(fake_buffer.get(), "tee_fake_sink")) != GST_PAD_LINK_OK) {
		throw std::runtime_error("Linking the fakesink tee pads failed");
	}
}

void DmpRadio::state_changed(std::string element, GstState, GstState, GstState)
{
	if(element == "tcp_bridge") {
		if(auto sp = server.lock()) {
			 sp->update_radio_state();
		} else {
			throw std::runtime_error("Server pointer in radio: " + name + " was invalid.");
		}
	}
}

void DmpRadio::listen()
{
	gst_element_set_state(pipeline.get(), GST_STATE_READY);
}

void DmpRadio::add_listener(std::string name)
{
	std::lock_guard<std::recursive_mutex> l(*gstreamer_mutex);
	if(branches.find(name) != branches.end()) {
		throw std::runtime_error(name + " was already listening to radio: " + this->name);
	}
	
	DmpRadioEndpoint listener(name, port_pool->allocate_number());
	
	GstPad* tee_pad = gst_element_request_pad(tee.get(), tee_src_pad_template.get(), nullptr, nullptr);
	
	if(!tee_pad) {
		throw std::runtime_error("failed to create a request pad from the tee");
	}
	
	auto branch_it = branches.emplace(
		name, 
		TeeBranch(gst_pad_get_name(tee_pad), std::unique_ptr<GstPad, GStreamerRequestPadDeleter>(tee_pad, {tee.get()}), std::move(listener))
	).first;
	
	branch_it->second.endpoint.play();
	
	gst_bin_add(GST_BIN(pipeline.get()), branch_it->second.endpoint.get_bin());
	gst_pad_link(branch_it->second.pad.get(), branch_it->second.endpoint.get_sink_pad());
}

void DmpRadio::remove_listener(std::string name)
{
	std::lock_guard<std::recursive_mutex> l(*gstreamer_mutex);

	auto branch_it = branches.find(name);
	if(branch_it == branches.end()) {
		throw std::runtime_error(name + " wasn't listening to radio: " + this->name);
	}

	gst_pad_unlink(branch_it->second.pad.get(), branch_it->second.endpoint.get_sink_pad());
	gst_element_set_state(branch_it->second.endpoint.get_bin(), GST_STATE_NULL);

	wait_for_state_change();

	gst_bin_remove(GST_BIN(pipeline.get()), branch_it->second.endpoint.get_bin());

	port_pool->free_number(branch_it->second.endpoint.get_port());

	branches.erase(branch_it);
}

void DmpRadio::disconnect(std::string endpoint_name)
{
	std::lock_guard<std::recursive_mutex> l(*gstreamer_mutex);
	
	if(branches.find(endpoint_name) != branches.end()) {
		remove_listener(endpoint_name);
	}
	
	GstState old_state = wait_for_state_change();
	
	auto it = playlist.begin();
	while(it != playlist.end())
	{
		if(it->owner == endpoint_name) {
			if(it == playlist.begin()) {
				stop();
			}
			it = playlist.erase(it);
		} else {
			++it;
		}
	}
	
	auto sp = server.lock();
	
	sp->update_playlist(name, playlist);
	
	if(old_state == GST_STATE_PLAYING) {
		play();
	}
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
	std::lock_guard<std::recursive_mutex> l(*gstreamer_mutex);

	auto sp = server.lock();

	gst_element_set_state(pipeline.get(), GST_STATE_READY);

	wait_for_state_change();

	for (auto&& branch : branches)
	{
		sp->forward_receiver_action(branch.first, message::ReceiverAction(name, message::PlaybackAction::Stop));
	}

	if(!playlist.empty()) {
		PlaylistEntry entry = playlist.front();
		sp->forward_sender_action(entry.owner, message::SenderAction(name, message::PlaybackAction::Stop));
	}

	stopped = true;
}

void DmpRadio:: play()
{
	std::lock_guard<std::recursive_mutex> l(*gstreamer_mutex);
	
	if (playlist.empty()) {
		return;
	}
	
	PlaylistEntry entry = playlist.front();
	auto sp = server.lock();
	
	gst_element_set_state(pipeline.get(), GST_STATE_PLAYING);

	if(stopped) {
		sp->order_stream(entry.owner, name, entry.folder_id, entry.entry, get_sender_port());
	}

	sp->forward_sender_action(entry.owner, message::SenderAction(name, message::PlaybackAction::Play));

	for(auto&& branch : branches) {
		sp->forward_receiver_action(branch.first, message::ReceiverAction(name, message::PlaybackAction::Play));
	}
	
	stopped = false;
}

void DmpRadio::pause()
{
	std::lock_guard<std::recursive_mutex> l(*gstreamer_mutex);
	
	auto sp = server.lock();

	if(playlist.empty()) {
		throw std::runtime_error("Playlist was empty but client was in playing state and attempted to pause.");
	}

	PlaylistEntry entry = playlist.front();
	
	sp->forward_sender_action(entry.owner, message::SenderAction(name, message::PlaybackAction::Pause));

	event_callbacks.clear();
	event_callbacks[message::PlaybackEvent::Paused] = [this, sp]{

		for(auto&& branch : branches)
		{
			sp->forward_receiver_action(branch.first, message::ReceiverAction(name, message::PlaybackAction::Pause));
		}

		gst_element_set_state(pipeline.get(), GST_STATE_PAUSED);
	};
}

void DmpRadio::next()
{
	std::lock_guard<std::recursive_mutex> l(*gstreamer_mutex);
	
	gst_element_set_state(pipeline.get(), GST_STATE_NULL);
	
	if(playlist.empty()) {
		return;
	}
	
	auto sp = server.lock();
	
	if(!stopped) {
		sp->forward_sender_action(playlist.front().owner, message::SenderAction(name, message::PlaybackAction::Reset));
		gst_element_set_state(pipeline.get(), GST_STATE_READY);
		
		wait_for_state_change();
	}
	
	playlist.erase(playlist.begin());
	
	if(!stopped) { 
		sp->update_playlist(name, playlist);
		stop();
		if(!playlist.empty()) {
			play();
		}
	}
}

uint32_t DmpRadio::queue(PlaylistEntry& pl_entry)
{
	std::lock_guard<std::recursive_mutex> l(*gstreamer_mutex);
	
	pl_entry.playlist_id = playlist_id++;
	playlist.push_back(pl_entry);
	return pl_entry.playlist_id;
}

void DmpRadio::unqueue(std::vector<PlaylistId> ids)
{
	std::lock_guard<std::recursive_mutex> l(*gstreamer_mutex);
	for(auto&& id : ids) {
		auto entry = playlist.end();
		for(auto it = playlist.begin(); it != playlist.end(); ++it) {
			if(it->playlist_id == id) {
				entry = it;
				break;
			}
		}
		if(entry == playlist.begin()) {
			next();
		} else {
			playlist.erase(entry);
		}
	}
}

void DmpRadio::move_up(std::vector<PlaylistId> ids)
{
	std::lock_guard<std::recursive_mutex> l(*gstreamer_mutex);
	bool move_up = false;
	std::vector<size_t> gathered;
	
	for(size_t i = 0; i < playlist.size(); ++i) {
		for(auto&& id : ids) {
			if(id == playlist[i].playlist_id) {
				if(move_up) {
					gathered.push_back(i);
				} 
			} else {
				if(!move_up) {
					move_up = true;
				}
			}
		}
	}
	
	bool stopandplay = false;
	for(auto&& index : gathered) {
		stopandplay = stopandplay || index - 1 == 0;
		std::swap(playlist[index - 1], playlist[index]);
	}
	
	if (stopandplay) {
		bool should_play = get_state().playing;
		stop();
		if(should_play) {
			play();
		}
	}
}

void DmpRadio::move_down(std::vector<PlaylistId> ids)
{
	std::lock_guard<std::recursive_mutex> l(*gstreamer_mutex);
	bool move_down = false;
	std::vector<size_t> gathered;
	
	for(size_t i = playlist.size() - 1; i != std::numeric_limits<size_t>::max(); --i) {
		for(auto&& id : ids) {
			if(id == playlist[i].playlist_id) {
				if(move_down) {
					gathered.push_back(i);
				} 
			} else {
				if(!move_down) {
					move_down = true;
				}
			}
		}
	}
	
	bool stopandplay = false;
	for(auto&& index : gathered) {
		stopandplay = stopandplay || index == 0;
		std::swap(playlist[index + 1], playlist[index]);
	}
	
	if (stopandplay) {
		bool should_play = get_state().playing;
		stop();
		if(should_play) {
			play();
		}
	}
}

RadioState DmpRadio::get_state()
{
	GstState state = wait_for_state_change();
	return RadioState(state == GST_STATE_PLAYING);
}