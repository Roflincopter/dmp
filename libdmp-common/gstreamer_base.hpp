#pragma once

#include <gst/gst.h>

#include <string>
#include <memory>

#define CHECK_GSTREAMER_COMPONENT(x) if(!x) std::cout << "Failed to create: " << #x << " element variable." << std::endl

void on_pad_added(_GstElement* element, _GstPad* pad, void* data);
gboolean bus_call (GstBus* bus, GstMessage* msg, gpointer data);

struct GMainLoopDeleter {
	void operator()(GMainLoop* ptr) {
		g_main_loop_unref(ptr);
	}
};

struct GStreamerObjectDeleter {
	void operator()(GstElement* ptr) {
		gst_object_unref(ptr);
	}
	
	void operator()(GstBus* ptr) {
		gst_object_unref(ptr);
	}
};

struct GStreamerEmptyDeleter {
	void operator()(GstElement*) {}
};

struct GStreamerBase {
	std::string name;
	
	std::unique_ptr<GMainLoop, GMainLoopDeleter> loop;
	
	std::unique_ptr<GstElement, GStreamerObjectDeleter> pipeline;
	std::unique_ptr<GstBus, GStreamerObjectDeleter> bus;
	
	guint gst_bus_watch_id;
	
	virtual void eos_reached();
	virtual void error_encountered(GError err);
	
	GStreamerBase(std::string name) 
	: name(name)
	, loop(g_main_loop_new(nullptr, false))
	, pipeline(gst_pipeline_new(name.c_str()))
	, bus(gst_pipeline_get_bus(GST_PIPELINE(pipeline.get())))
	, gst_bus_watch_id(gst_bus_add_watch(bus.get(), bus_call, this))
	{}
	
	GStreamerBase(GStreamerBase&& base)
	: name(std::move(base.name))
	, loop(std::move(base.loop))
	, pipeline(std::move(base.pipeline))
	, bus(std::move(base.bus))
	, gst_bus_watch_id(std::move(base.gst_bus_watch_id))
	{
		base.name = "";
		base.loop.reset();
		base.pipeline.reset();
		base.bus.reset();
		base.gst_bus_watch_id = 0;
		
		g_source_remove(gst_bus_watch_id);
		gst_bus_watch_id = gst_bus_add_watch(bus.get(), bus_call, this);
	}
	
	void run_loop();
	void stop_loop();
	void make_debug_graph();
};
