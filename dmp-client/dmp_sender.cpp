#include "dmp_sender.hpp"

#include "gstreamer_util.hpp"

#include <stdexcept>
#include <iostream>

DmpSender::DmpSender()
: loop(nullptr)
, pipeline(nullptr)
, source(nullptr)
, decoder(nullptr)
, encoder(nullptr)
, sink(nullptr)
, bus(nullptr)
{
	loop = g_main_loop_new(nullptr, false);

	pipeline = gst_pipeline_new("sender");
	source   = gst_element_factory_make("filesrc", "file");
	decoder  = gst_element_factory_make("decodebin", "decoder");
	encoder  = gst_element_factory_make("lamemp3enc", "encoder");
	sink     = gst_element_factory_make("tcpclientsink", "send");

	if (!pipeline || !source || !decoder || !encoder || !sink)
	{
		CHECK_GSTREAMER_COMPONENT(pipeline);
		CHECK_GSTREAMER_COMPONENT(source);
		CHECK_GSTREAMER_COMPONENT(decoder);
		CHECK_GSTREAMER_COMPONENT(encoder);
		CHECK_GSTREAMER_COMPONENT(sink);
		throw std::runtime_error("Could not create the pipeline components for this sender.");
	}

	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_add_watch (bus, bus_call, loop);

	gst_bin_add_many (GST_BIN(pipeline), source, decoder, encoder, sink, nullptr);
	gst_element_link_many(source, decoder, nullptr);
	g_signal_connect(decoder, "pad-added", G_CALLBACK(on_pad_added), encoder);
	gst_element_link_many(encoder, sink, nullptr);

	GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "dmp_sender");
}

void DmpSender::run(std::string host, uint16_t port, std::string file){
	g_object_set(G_OBJECT(source), "location", file.c_str(), nullptr);

	g_object_set(G_OBJECT(encoder), "bitrate", gint(320), nullptr);
	g_object_set(G_OBJECT(encoder), "cbr", gboolean(true), nullptr);

	g_object_set(G_OBJECT(sink), "host", host.c_str(), nullptr);
	g_object_set(G_OBJECT(sink), "port", gint(port), nullptr);
	
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	g_main_loop_run(loop);
}
