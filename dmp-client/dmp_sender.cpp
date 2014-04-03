#include "dmp_sender.hpp"

#include <stdexcept>
#include <iostream>

DmpSender::DmpSender()
: loop(g_main_loop_new(nullptr, false))
, pipeline(gst_pipeline_new("sender"))
, source(gst_element_factory_make("filesrc", "file"))
, decoder(gst_element_factory_make("decodebin", "decoder"))
, encoder(gst_element_factory_make("lamemp3enc", "encoder"))
, sink(gst_element_factory_make("tcpclientsink", "send"))
{
	if (!pipeline || !source || !decoder || !encoder || !sink)
	{
		CHECK_GSTREAMER_COMPONENT(pipeline);
		CHECK_GSTREAMER_COMPONENT(source);
		CHECK_GSTREAMER_COMPONENT(decoder);
		CHECK_GSTREAMER_COMPONENT(encoder);
		CHECK_GSTREAMER_COMPONENT(sink);
		throw std::runtime_error("Could not create the pipeline components for this sender.");
	}
	
	bus.reset(gst_pipeline_get_bus (GST_PIPELINE (pipeline.get())));
	add_bus_callbacks();

	gst_bin_add_many (GST_BIN(pipeline.get()), source.get(), decoder.get(), encoder.get(), sink.get(), nullptr);
	gst_element_link_many(source.get(), decoder.get(), nullptr);
	g_signal_connect(decoder.get(), "pad-added", G_CALLBACK(on_pad_added), encoder.get());
	gst_element_link_many(encoder.get(), sink.get(), nullptr);

	GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline.get()), GST_DEBUG_GRAPH_SHOW_ALL, "dmp_sender");
}

DmpSender::~DmpSender()
{
	gst_element_set_state(pipeline.get(), GST_STATE_NULL);
	g_main_loop_quit(loop.get());
}

void DmpSender::run(std::string host, uint16_t port, std::string file){
	g_object_set(G_OBJECT(source.get()), "location", file.c_str(), nullptr);

	g_object_set(G_OBJECT(encoder.get()), "bitrate", gint(320), nullptr);
	g_object_set(G_OBJECT(encoder.get()), "cbr", gboolean(true), nullptr);

	g_object_set(G_OBJECT(sink.get()), "host", host.c_str(), nullptr);
	g_object_set(G_OBJECT(sink.get()), "port", gint(port), nullptr);
	
	gst_element_set_state(pipeline.get(), GST_STATE_READY);

	g_main_loop_run(loop.get());
}

void DmpSender::pause()
{
	gst_element_set_state(pipeline.get(), GST_STATE_PAUSED);
}

void DmpSender::play()
{
	gst_element_set_state(pipeline.get(), GST_STATE_PLAYING);
}

void DmpSender::stop()
{
	gst_element_set_state(pipeline.get(), GST_STATE_READY);
}
