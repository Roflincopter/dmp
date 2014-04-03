
#include "gstreamer_base.hpp"

#include <iostream>
#include <cassert>

void GStreamerBase::add_bus_callbacks()
{
	if(gst_bus_watch_id) {
		g_source_remove(gst_bus_watch_id);
	}
	if(bus) {
		gst_bus_watch_id = gst_bus_add_watch(bus.get(), bus_call, this);
	}
}

void GStreamerBase::eos_reached()
{
	std::cerr << "End of stream Reached." << std::endl;
}

void GStreamerBase::error_encountered(GError err)
{
	std::cerr << std::string(err.message) << std::endl;
}

gboolean bus_call (GstBus* bus, GstMessage* msg, gpointer data)
{
	GStreamerBase* base = static_cast<GStreamerBase*>(data);
	
	switch (GST_MESSAGE_TYPE (msg)) {

	case GST_MESSAGE_EOS:
		base->eos_reached();
		break;

	case GST_MESSAGE_ERROR: {
		gchar  *debug;
		GError *error;

		gst_message_parse_error (msg, &error, &debug);
		g_free (debug);

		base->error_encountered(*error);
		g_error_free (error);
		break;
	}
	default:
		break;
	}
	return true;
}

void on_pad_added (GstElement* element, GstPad* pad, gpointer data)
{
  GstPad *sinkpad;
  GstElement *decoder = (GstElement *) data;

  /* We can now link this pad with the vorbis-decoder sink pad */
  g_print ("Dynamic pad created, linking demuxer/decoder\n");

  sinkpad = gst_element_get_static_pad (decoder, "sink");

  gst_pad_link (pad, sinkpad);

  gst_object_unref (sinkpad);
}