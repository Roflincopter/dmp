#pragma once

#include <gst/gst.h>

#include <string>

struct GStreamerBase {
	virtual void eos_reached();;
	virtual void error_encountered(GError err);
};

#define CHECK_GSTREAMER_COMPONENT(x) if(!x) std::cout << "Failed to create: " << #x << " element variable." << std::endl

void on_pad_added(_GstElement* element, _GstPad* pad, void* data);
gboolean bus_call (GstBus* bus, GstMessage* msg, gpointer data);
