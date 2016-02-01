#include "dmp_receiver.hpp"

#include <glib/gtypes.h>

#include <gobject/gclosure.h>
#include <gobject/gobject.h>
#include <gobject/gsignal.h>

#include <gst/gstbin.h>
#include <gst/gstelement.h>
#include <gst/gstelementfactory.h>
#include <gst/gstutils.h>

#include <stdexcept>
#include <iostream>

void DmpReceiver::state_changed(std::string, GstState, GstState, GstState)
{}

DmpReceiver::DmpReceiver(std::string gst_dir)
: GStreamerBase("receiver", gst_dir)
, source(gst_element_factory_make("tcpclientsrc", "stream"))
, buffer(gst_element_factory_make("queue2", "buffer"))
, decoder(gst_element_factory_make("decodebin", "decoder"))
, converter(gst_element_factory_make("audioconvert", "converter"))
, resampler(gst_element_factory_make("audioresample", "resampler"))
, volume(gst_element_factory_make("volume", "volume"))
, audiosink(gst_element_factory_make("autoaudiosink", "audiosink"))
, radio_name()
{
	if (!source || !buffer || !decoder || !converter || !resampler || !volume || !audiosink)
	{
		CHECK_GSTREAMER_COMPONENT(source);
		CHECK_GSTREAMER_COMPONENT(buffer);
		CHECK_GSTREAMER_COMPONENT(decoder);
		CHECK_GSTREAMER_COMPONENT(converter);
		CHECK_GSTREAMER_COMPONENT(resampler);
		CHECK_GSTREAMER_COMPONENT(volume);
		CHECK_GSTREAMER_COMPONENT(audiosink);
		throw std::runtime_error("Could not create the pipeline components for this receiver.");
	}
	
	g_object_set(G_OBJECT(buffer.get()), "max-size-time", gint(30000000000), nullptr);
	g_object_set(G_OBJECT(buffer.get()), "use-buffering", gboolean(true), nullptr);
	
	gst_bin_add_many(GST_BIN(pipeline.get()), source.get(), /*rtpdepay.get(),*/ buffer.get(), decoder.get(), converter.get(), resampler.get(), volume.get(), audiosink.get(), nullptr);
	
	gst_element_link_many(source.get(), /*rtpdepay.get(),*/ buffer.get(), decoder.get(), nullptr);
	g_signal_connect(decoder.get(), "pad-added", G_CALLBACK(on_pad_added), converter.get());
	gst_element_link_many(converter.get(), resampler.get(), volume.get(), audiosink.get(), nullptr);
}

DmpReceiver::~DmpReceiver()
{
	destroy();
}

void DmpReceiver::eos_reached()
{}

void DmpReceiver::setup(std::string name, std::string host, uint16_t port) {
	radio_name = name;
	
	gst_element_set_state(pipeline.get(), GST_STATE_NULL);
	
	g_object_set(G_OBJECT(source.get()), "host", host.c_str(), nullptr);
	g_object_set(G_OBJECT(source.get()), "port", gint(port), nullptr);
	
	gst_element_set_state(pipeline.get(), GST_STATE_READY);
}

void DmpReceiver::play()
{
	std::cout << "before playing" << std::endl;
	if(!gst_element_set_state(pipeline.get(), GST_STATE_PLAYING))
	{
		throw std::runtime_error("Could not change receiver state to playing");
	}
	std::cout << "after playing" << std::endl;
}

void DmpReceiver::pause()
{
	gst_element_set_state(pipeline.get(), GST_STATE_PAUSED);
}

void DmpReceiver::stop()
{
	gst_element_set_state(pipeline.get(), GST_STATE_READY);
}

void DmpReceiver::mute(bool mute)
{
	g_object_set(G_OBJECT(volume.get()), "mute", gboolean(mute), nullptr);
}

void DmpReceiver::set_volume(int ivolume)
{
	dvolume = ivolume / 100.0;
	g_object_set(G_OBJECT(volume.get()), "volume", gdouble(dvolume), nullptr);
}

int DmpReceiver::get_volume()
{
	return dvolume * 100;
}

std::string DmpReceiver::radio_target()
{
	return radio_name;
}
