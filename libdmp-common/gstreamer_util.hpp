#pragma once

#include <gst/gst.h>

void on_pad_added(_GstElement* element, _GstPad* pad, void* data);
gboolean bus_call (GstBus* bus, GstMessage* msg, gpointer data);
