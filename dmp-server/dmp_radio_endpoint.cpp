#include "dmp_radio_endpoint.hpp"

#include "gstreamer_base.hpp"

#include <glib/gtypes.h>

#include <gobject/gobject.h>

#include <gst/gstbin.h>
#include <gst/gstelement.h>
#include <gst/gstelementfactory.h>
#include <gst/gstghostpad.h>
#include <gst/gstpad.h>
#include <gst/gstutils.h>

DmpRadioEndpoint::DmpRadioEndpoint(std::string name, uint16_t port)
: name(name)
, port(port)
, bin(gst_bin_new((name + std::string("_sink_") + std::to_string(port)).c_str()))
, buffer(gst_element_factory_make("queue2", "buffer"))
, sink(gst_element_factory_make("tcpserversink", (name + "_sink").c_str()))
{
	g_object_set(G_OBJECT(sink.get()), "host", "0.0.0.0", nullptr);
	g_object_set(G_OBJECT(sink.get()), "port", gint(port), nullptr);
	
	g_object_set(G_OBJECT(buffer.get()), "max-size-time", gint(30000000000), nullptr);
	g_object_set(G_OBJECT(buffer.get()), "use-buffering", gboolean(true), nullptr);

	gst_bin_add_many(GST_BIN(bin.get()), buffer.get(), sink.get(), nullptr);
	
	gst_element_link_many(buffer.get(), sink.get(), nullptr);
	
	gst_element_add_pad(bin.get(), gst_ghost_pad_new("tee_sink", gst_element_get_static_pad(buffer.get(), "sink")));
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
	return gst_element_get_static_pad(bin.get(), "tee_sink");
}

GstElement* DmpRadioEndpoint::get_bin()
{
	return bin.get();
}

uint16_t DmpRadioEndpoint::get_port()
{
	return port;
}


