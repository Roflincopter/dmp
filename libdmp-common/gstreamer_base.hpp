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
	bool buffering;
	
	std::string name;
	
	std::unique_ptr<GMainLoop, GMainLoopDeleter> loop;
	
	std::unique_ptr<GstElement, GStreamerObjectDeleter> pipeline;
	std::unique_ptr<GstBus, GStreamerObjectDeleter> bus;
	
	guint gst_bus_watch_id;
	
	virtual void eos_reached();
	virtual void error_encountered(std::string pipeline, std::string element, GError err);
	virtual void buffer_high();
	virtual void buffer_low();
	
	GStreamerBase(std::string name);
	
	GStreamerBase(GStreamerBase&& base);
	
	virtual ~GStreamerBase();
	
	void run_loop();
	void stop_loop();
	void make_debug_graph();
};
