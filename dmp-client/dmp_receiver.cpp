#include "dmp_receiver.hpp"

#include "debug_macros.hpp"

#include <stdexcept>
#include <iostream>
#include <cassert>

DmpReceiver::DmpReceiver()
: GStreamerBase("receiver")
, source(gst_element_factory_make("tcpclientsrc", "stream"))
//, rtpdepay(gst_element_factory_make("rtpmpadepay", "rtpdepay"))
, buffer(gst_element_factory_make("queue2", "buffer"))
, decoder(gst_element_factory_make("decodebin", "decoder"))
, converter(gst_element_factory_make("audioconvert", "converter"))
, resampler(gst_element_factory_make("audioresample", "resampler"))
, volume(gst_element_factory_make("volume", "volume"))
, audiosink(gst_element_factory_make("autoaudiosink", "audiosink"))
, radio_name()
{
	if (!source || /*!rtpdepay ||*/ !buffer || !decoder || !converter || !resampler || !volume || !audiosink)
	{
		CHECK_GSTREAMER_COMPONENT(source);
//		CHECK_GSTREAMER_COMPONENT(rtpdepay);
		CHECK_GSTREAMER_COMPONENT(buffer);
		CHECK_GSTREAMER_COMPONENT(decoder);
		CHECK_GSTREAMER_COMPONENT(converter);
		CHECK_GSTREAMER_COMPONENT(resampler);
		CHECK_GSTREAMER_COMPONENT(volume);
		CHECK_GSTREAMER_COMPONENT(audiosink);
		throw std::runtime_error("Could not create the pipeline components for this receiver.");
	}
	
	g_object_set(G_OBJECT(buffer.get()), "max-size-time", gint(100000000), nullptr);
	g_object_set(G_OBJECT(buffer.get()), "use-buffering", gboolean(true), nullptr);
	
	gst_bin_add_many(GST_BIN(pipeline.get()), source.get(), /*rtpdepay.get(),*/ buffer.get(), decoder.get(), converter.get(), resampler.get(), volume.get(), audiosink.get(), nullptr);
	
	gst_element_link_many(source.get(), /*rtpdepay.get(),*/ buffer.get(), decoder.get(), nullptr);
	g_signal_connect(decoder.get(), "pad-added", G_CALLBACK(on_pad_added), converter.get());
	gst_element_link_many(converter.get(), resampler.get(), volume.get(), audiosink.get(), nullptr);
}

void DmpReceiver::eos_reached()
{
	DEBUG_COUT << "DmpReceiver: EOS reached" << std::endl;
	gst_element_set_state(pipeline.get(), GST_STATE_NULL);
}

void DmpReceiver::setup(std::string name, std::string host, uint16_t port) {
	radio_name = name;
	
	gst_element_set_state(pipeline.get(), GST_STATE_NULL);
	
	g_object_set(G_OBJECT(source.get()), "host", host.c_str(), nullptr);
	g_object_set(G_OBJECT(source.get()), "port", gint(port), nullptr);
	
	gst_element_set_state(pipeline.get(), GST_STATE_READY);
}

void DmpReceiver::play()
{
	DEBUG_COUT << "Play called on receiver" << std::endl;
	gst_element_set_state(pipeline.get(), GST_STATE_PLAYING);
}

void DmpReceiver::pause()
{
	DEBUG_COUT << "Pause called on receiver" << std::endl;
	gst_element_set_state(pipeline.get(), GST_STATE_PAUSED);
}

void DmpReceiver::stop()
{
	DEBUG_COUT << "Stop called on receiver" << std::endl;
	gst_element_set_state(pipeline.get(), GST_STATE_READY);
}

void DmpReceiver::mute(bool mute)
{
	g_object_set(G_OBJECT(volume.get()), "mute", gboolean(mute), nullptr);
}

std::string DmpReceiver::radio_target()
{
	DEBUG_COUT << "Getting the radio target name: " << radio_name << std::endl;
	return radio_name;
}
