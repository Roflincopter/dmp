#include "dmp_receiver.hpp"

#include <stdexcept>
#include <iostream>

DmpReceiver::DmpReceiver()
: loop(nullptr)
, pipeline(nullptr)
, source(nullptr)
, decoder(nullptr)
, converter(nullptr)
, resampler(nullptr)
, audiosink(nullptr)
{
	loop = g_main_loop_new(nullptr, false);

	pipeline  = gst_pipeline_new("receiver");
	source    = gst_element_factory_make("tcpclientsrc", "stream");
	decoder   = gst_element_factory_make("decodebin", "decoder");
	converter = gst_element_factory_make("audioconvert", "converter");
	resampler = gst_element_factory_make("audioresample", "resampler");
	audiosink = gst_element_factory_make("autoaudiosink", "audiosink");

	if (!pipeline || !source || !decoder || !converter || !resampler || !audiosink)
	{
		CHECK_GSTREAMER_COMPONENT(pipeline);
		CHECK_GSTREAMER_COMPONENT(source);
		CHECK_GSTREAMER_COMPONENT(decoder);
		CHECK_GSTREAMER_COMPONENT(converter);
		CHECK_GSTREAMER_COMPONENT(resampler);
		CHECK_GSTREAMER_COMPONENT(audiosink);
		throw std::runtime_error("Could not create the pipeline components for this receiver.");
	}

	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_add_watch (bus, bus_call, this);

	gst_bin_add_many(GST_BIN(pipeline), source, decoder, converter, resampler, audiosink, nullptr);
	gst_element_link_many(source, decoder, nullptr);
	g_signal_connect(decoder, "pad-added", G_CALLBACK(on_pad_added), converter);
	gst_element_link_many(converter, resampler, audiosink, nullptr);
}

void DmpReceiver::stop()
{
	g_main_loop_quit(loop);
}

void DmpReceiver::connect(std::string host, uint16_t port)
{
	gst_element_set_state(pipeline, GST_STATE_NULL);

	g_object_set(G_OBJECT(source), "host", host.c_str(), nullptr);
	g_object_set(G_OBJECT(source), "port", gint(port), nullptr);

	GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "dmp_receiver");

	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	g_main_loop_run(loop);
}

