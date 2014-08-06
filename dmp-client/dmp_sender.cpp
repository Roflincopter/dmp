#include "dmp_sender.hpp"

#include <stdexcept>
#include <iostream>

#include "debug_macros.hpp"

DmpSender::DmpSender(std::weak_ptr<DmpClientRadioInterface> client, std::string radio_name)
: GStreamerBase("sender")
, client(client)
, radio_name(radio_name)
, source(gst_element_factory_make("filesrc", "file"))
, decoder(gst_element_factory_make("decodebin", "decoder"))
, converter(gst_element_factory_make("audioconvert", "converter"))
, resampler(gst_element_factory_make("audioresample", "resampler"))
, encoder(gst_element_factory_make("lamemp3enc", "encoder"))
, buffer(gst_element_factory_make("queue2", "buffer"))
, sink(gst_element_factory_make("tcpclientsink", "send"))
{
	if (!source || !decoder || !converter || !resampler || !encoder || /*!rtppay ||*/ !buffer || !sink)
	{
		CHECK_GSTREAMER_COMPONENT(source);
		CHECK_GSTREAMER_COMPONENT(decoder);
		CHECK_GSTREAMER_COMPONENT(converter);
		CHECK_GSTREAMER_COMPONENT(resampler);
		CHECK_GSTREAMER_COMPONENT(encoder);
		CHECK_GSTREAMER_COMPONENT(buffer);
		CHECK_GSTREAMER_COMPONENT(sink);
		throw std::runtime_error("Could not create the pipeline components for this sender.");
	}

	gst_bin_add_many (GST_BIN(pipeline.get()), source.get(), decoder.get(), converter.get(), resampler.get(), encoder.get(), /*rtppay.get(),*/ buffer.get(), sink.get(), nullptr);
	
	gst_element_link_many(source.get(), decoder.get(), nullptr);
	g_signal_connect(decoder.get(), "pad-added", G_CALLBACK(on_pad_added), converter.get());
	gst_element_link_many(converter.get(), resampler.get(), encoder.get(), /*rtppay.get(),*/ buffer.get(), sink.get(), nullptr);
}

void DmpSender::setup(std::string host, uint16_t port, std::string file)
{
	g_object_set(G_OBJECT(source.get()), "location", file.c_str(), nullptr);

	g_object_set(G_OBJECT(encoder.get()), "bitrate", gint(320), nullptr);
	g_object_set(G_OBJECT(encoder.get()), "cbr", gboolean(true), nullptr);

	g_object_set(G_OBJECT(sink.get()), "host", host.c_str(), nullptr);
	g_object_set(G_OBJECT(sink.get()), "port", gint(port), nullptr);

	g_object_set(G_OBJECT(buffer.get()), "max-size-time", gint(100000000), nullptr);
	g_object_set(G_OBJECT(buffer.get()), "use-buffering", gboolean(true), nullptr);
	
	gst_element_set_state(pipeline.get(), GST_STATE_READY);
}

void DmpSender::eos_reached()
{
	gst_element_set_state(pipeline.get(), GST_STATE_NULL);
	if(!is_resetting) {
		client.lock()->forward_radio_action(message::RadioAction(radio_name, message::PlaybackAction::Next));
	}
	is_resetting = false;
}

void DmpSender::pause()
{
	if(!gst_element_set_state(pipeline.get(), GST_STATE_PAUSED)) {
		throw std::runtime_error("Could not pause the Sender pipeline");
	}
	wait_for_state_change();

	auto cp = client.lock();
	cp->forward_radio_event(message::SenderEvent(radio_name, message::PlaybackEvent::Paused));
}

void DmpSender::play()
{
	if(!gst_element_set_state(pipeline.get(), GST_STATE_PLAYING)) {
		throw std::runtime_error("Could not play the Sender pipeline");
	}
}

void DmpSender::stop()
{
	if(!gst_element_set_state(pipeline.get(), GST_STATE_NULL)) {
		throw std::runtime_error("Could not stop the Sender pipeline");
	}
}

void DmpSender::reset()
{
	is_resetting = true;
	gst_element_set_state(pipeline.get(), GST_STATE_NULL);
}
