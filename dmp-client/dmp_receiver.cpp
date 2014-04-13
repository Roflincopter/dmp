#include "dmp_receiver.hpp"

#include <stdexcept>
#include <iostream>

DmpReceiver::DmpReceiver()
: GStreamerBase("receiver")
, source(gst_element_factory_make("tcpclientsrc", "stream"))
//, rtpdepay(gst_element_factory_make("rtpmpadepay", "rtpdepay"))
, buffer(gst_element_factory_make("queue2", "buffer"))
, decoder(gst_element_factory_make("decodebin", "decoder"))
, converter(gst_element_factory_make("audioconvert", "converter"))
, resampler(gst_element_factory_make("audioresample", "resampler"))
, audiosink(gst_element_factory_make("autoaudiosink", "audiosink"))
{
	if (!source || /*!rtpdepay ||*/ !buffer || !decoder || !converter || !resampler || !audiosink)
	{
		CHECK_GSTREAMER_COMPONENT(source);
//		CHECK_GSTREAMER_COMPONENT(rtpdepay);
		CHECK_GSTREAMER_COMPONENT(buffer);
		CHECK_GSTREAMER_COMPONENT(decoder);
		CHECK_GSTREAMER_COMPONENT(converter);
		CHECK_GSTREAMER_COMPONENT(resampler);
		CHECK_GSTREAMER_COMPONENT(audiosink);
		throw std::runtime_error("Could not create the pipeline components for this receiver.");
	}

	g_object_set(G_OBJECT(buffer.get()), "max-size-time", gint(100000000), nullptr);
	g_object_set(G_OBJECT(buffer.get()), "use-buffering", gboolean(true), nullptr);
	
	gst_bin_add_many(GST_BIN(pipeline.get()), source.get(), /*rtpdepay.get(),*/ buffer.get(), decoder.get(), converter.get(), resampler.get(), audiosink.get(), nullptr);
	
	gst_element_link_many(source.get(), /*rtpdepay.get(),*/ buffer.get(), decoder.get(), nullptr);
	g_signal_connect(decoder.get(), "pad-added", G_CALLBACK(on_pad_added), converter.get());
	gst_element_link_many(converter.get(), resampler.get(), audiosink.get(), nullptr);
}

void DmpReceiver::eos_reached()
{
	gst_element_set_state(pipeline.get(), GST_STATE_NULL);
	gst_element_set_state(pipeline.get(), GST_STATE_PLAYING);
}

void DmpReceiver::setup(std::string host, uint16_t port) {
	gst_element_set_state(pipeline.get(), GST_STATE_NULL);
	
	g_object_set(G_OBJECT(source.get()), "host", host.c_str(), nullptr);
	g_object_set(G_OBJECT(source.get()), "port", gint(port), nullptr);
	
	gst_element_set_state(pipeline.get(), GST_STATE_READY);
}

void DmpReceiver::play()
{
	gst_element_set_state(pipeline.get(), GST_STATE_PLAYING);
}
