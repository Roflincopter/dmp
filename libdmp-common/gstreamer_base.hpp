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
	guint gst_bus_watch_id;
	
	std::unique_ptr<GMainLoop, GMainLoopDeleter> loop;
	std::unique_ptr<GstBus, GStreamerObjectDeleter> bus;
	
	virtual void eos_reached();
	virtual void error_encountered(GError err);
	
	GStreamerBase() 
	: gst_bus_watch_id(0)
	, loop(g_main_loop_new(nullptr, false))
	, bus(nullptr)
	{
		add_bus_callbacks();
	}
	
	GStreamerBase(GStreamerBase&& base)
	: gst_bus_watch_id(std::move(base.gst_bus_watch_id))
	, loop(std::move(base.loop))
	, bus(std::move(base.bus))
	{
		base.gst_bus_watch_id = 0;
		base.loop.reset();
		base.bus.reset();
		
		add_bus_callbacks();
	}
	
	void run_loop();
	void stop_loop();
	
protected: 
	void add_bus_callbacks();
};
