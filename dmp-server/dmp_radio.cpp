#include "dmp_radio.hpp"

#include "gstreamer_util.hpp"

DmpRadio::DmpRadio(uint16_t from, uint16_t to)
{
	GMainLoop* loop;

	GstElement* pipeline = nullptr,* source = nullptr,* sink = nullptr;
	GstBus *bus = nullptr;

	std::cout << "Initializing gstreamer." << std::endl;

	gst_init(0, nullptr);

	std::cout << "Initialization complete." << std::endl;

	loop = g_main_loop_new(nullptr, false);

	pipeline = gst_pipeline_new("tcp_bridge");
	source   = gst_element_factory_make("tcpserversrc", "recv");
	sink     = gst_element_factory_make("tcpserversink", "send");

	std::cout << "Checking element validity." << std::endl;

	if (!pipeline || !source || !sink)
	{
		CHECK_GSTREAMER_COMPONENT(pipeline);
		CHECK_GSTREAMER_COMPONENT(source);
		CHECK_GSTREAMER_COMPONENT(sink);
		std::cout << "throwing." << std::endl;
		throw std::runtime_error("Could not create the pipeline components for this radio.");
	}

	std::cout << "setting element arguments" << std::endl;

	g_object_set(G_OBJECT(source), "port", gint(from), nullptr);
	g_object_set(G_OBJECT(sink), "port", gint(to), nullptr);

	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_add_watch (bus, bus_call, loop);
	gst_object_unref (bus);

	gst_bin_add_many (GST_BIN(pipeline), source, sink, nullptr);
	gst_element_link_many(source, sink, nullptr);

	GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline), GST_DEBUG_GRAPH_SHOW_ALL, "dmp_radio");

	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	std::cout << "running main loop." << std::endl;
	g_main_loop_run(loop);
}
