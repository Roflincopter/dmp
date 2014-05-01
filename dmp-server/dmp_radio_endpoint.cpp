#include "dmp_radio_endpoint.hpp"

#include <iostream>

DmpRadioEndpoint::DmpRadioEndpoint(std::string name, uint16_t port)
: name(name)
, port(port)
, bin(gst_bin_new((name + std::string("_sink_") + std::to_string(port)).c_str()))
, sink(gst_element_factory_make("tcpserversink", (name + "_sink").c_str()))
{
	DEBUG_COUT << "sink ptr: " << sink.get() << std::endl;
	
	g_object_set(G_OBJECT(sink.get()), "host", "0.0.0.0", nullptr);
	g_object_set(G_OBJECT(sink.get()), "port", gint(port), nullptr);
	
	gst_bin_add_many(GST_BIN(bin.get()), sink.get(), nullptr);
	
	gst_element_add_pad(bin.get(), gst_ghost_pad_new("sink", gst_element_get_static_pad(sink.get(), "sink")));
}

void DmpRadioEndpoint::play()
{
	gst_element_set_state(bin.get(), GST_STATE_PLAYING);
}

GstElement*DmpRadioEndpoint::get_sink()
{
	return sink.get();
}

GstPad* DmpRadioEndpoint::get_sink_pad()
{
	return gst_element_get_static_pad(bin.get(), "sink");
}

GstElement* DmpRadioEndpoint::get_bin()
{
	return bin.get();
}

uint16_t DmpRadioEndpoint::get_port()
{
	return port;
}


