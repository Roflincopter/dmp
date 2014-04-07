
#include "gstreamer_base.hpp"

#include <iostream>
#include <cassert>

void GStreamerBase::eos_reached()
{
	std::cerr << "End of stream Reached." << std::endl;
}

void GStreamerBase::error_encountered(GError err)
{
	std::cerr << std::string(err.message) << std::endl;
}

GStreamerBase::GStreamerBase(GStreamerBase&& base)
: name(std::move(base.name))
, loop(std::move(base.loop))
, pipeline(std::move(base.pipeline))
, bus(std::move(base.bus))
, gst_bus_watch_id(std::move(base.gst_bus_watch_id))
{
	base.name = "<INVALID>";
	base.loop.reset();
	base.pipeline.reset();
	base.bus.reset();
	base.gst_bus_watch_id = 0;
	
	g_source_remove(gst_bus_watch_id);
	gst_bus_watch_id = gst_bus_add_watch(bus.get(), bus_call, this);
}

GStreamerBase::GStreamerBase(std::string name) 
: name(name)
, loop(g_main_loop_new(nullptr, false))
, pipeline(gst_pipeline_new(name.c_str()))
, bus(gst_pipeline_get_bus(GST_PIPELINE(pipeline.get())))
, gst_bus_watch_id(gst_bus_add_watch(bus.get(), bus_call, this))
{}

GStreamerBase::~GStreamerBase()
{
	if(pipeline) {
		gst_element_set_state(pipeline.get(), GST_STATE_NULL);
	}
	if(loop) {
		stop_loop();
	}
}

void GStreamerBase::run_loop()
{
	g_main_loop_run(loop.get());
}

void GStreamerBase::stop_loop()
{
	g_main_loop_quit(loop.get());
}

void GStreamerBase::make_debug_graph()
{
	GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline.get()), GST_DEBUG_GRAPH_SHOW_ALL, name.c_str());
}

gboolean bus_call (GstBus* bus, GstMessage* msg, gpointer data)
{
	GStreamerBase* base = static_cast<GStreamerBase*>(data);
	
	switch (GST_MESSAGE_TYPE (msg)) {

	case GST_MESSAGE_EOS: {
		base->eos_reached();
		break;
	}
		
	case GST_MESSAGE_STATE_CHANGED: {
		GstState old_;
		GstState new_;
		GstState pending;
		
		auto f = [](GstState x) {
			if(x == 0) return "GST_STATE_VOID_PENDING";
			if(x == 1) return "GST_STATE_NULL";
			if(x == 2) return "GST_STATE_READY";
			if(x == 3) return "GST_STATE_PAUSED";
			if(x == 4) return "GST_STATE_PLAYING";
			return "<UNKNOWN>";
		};
		
		gst_message_parse_state_changed(msg, &old_, &new_, &pending);
		
		std::cerr << base->name << " from: " << f(old_) << " to: " << f(new_) << " pending?: "  << f(pending) << std::endl;
		break;
	}

	case GST_MESSAGE_ERROR: {
		gchar  *debug_ptr;
		GError *error_ptr;
	
		gst_message_parse_error (msg, &error_ptr, &debug_ptr);
		
		GError error = *error_ptr;
		std::string debug(debug_ptr);
		
		g_free (debug_ptr);
		g_error_free(error_ptr);
		
		base->error_encountered(error);
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

  sinkpad = gst_element_get_static_pad (decoder, "sink");
  gst_pad_link (pad, sinkpad);
  gst_object_unref (sinkpad);
}
