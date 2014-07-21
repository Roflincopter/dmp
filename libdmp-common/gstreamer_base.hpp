#pragma once

#include <gst/gst.h>

#include <string>
#include <memory>

#define CHECK_GSTREAMER_COMPONENT(x) if(!x) std::cout << "Failed to create: " << #x << " element variable." << std::endl

#ifdef __MINGW32__
extern "C" {
// Must be done at global file scope, since this results in a function
// prototype being declared.
GST_PLUGIN_STATIC_DECLARE(coreelements);
GST_PLUGIN_STATIC_DECLARE(tcp);
GST_PLUGIN_STATIC_DECLARE(playback);
GST_PLUGIN_STATIC_DECLARE(audioconvert);
GST_PLUGIN_STATIC_DECLARE(audioresample);
GST_PLUGIN_STATIC_DECLARE(audioparsers);
GST_PLUGIN_STATIC_DECLARE(autodetect);
GST_PLUGIN_STATIC_DECLARE(lame);
}
#endif

void on_pad_added(_GstElement* element, _GstPad* pad, void* data);
gboolean bus_call (GstBus* bus, GstMessage* msg, gpointer data);
std::string gst_state_to_string(GstState x);

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
	
	void operator()(GstBin* ptr) {
		gst_object_unref(ptr);
	}
	
	void operator()(GstPadTemplate* ptr) {
		gst_object_unref(ptr);
	}
};

struct GStreamerRequestPadDeleter {
	
	GstElement* element;
	
	GStreamerRequestPadDeleter(GstElement* element)
	: element(element)
	{}
	
	void operator()(GstPad* ptr) {
		gst_element_release_request_pad(element, ptr);
	}
};

struct GStreamerEmptyDeleter {
	void operator()(GstElement*) {}
};

struct GErrorDeleter {
	void operator()(GError* ptr) {
		g_error_free(ptr);
	}
};

struct GStreamerBase { 
	bool buffering;
	
	std::string name;
	
	std::unique_ptr<GMainLoop, GMainLoopDeleter> loop;
	
	std::unique_ptr<GstElement, GStreamerObjectDeleter> pipeline;
	std::unique_ptr<GstBus, GStreamerObjectDeleter> bus;
	
	guint gst_bus_watch_id;
	
	virtual void eos_reached();
	virtual void error_encountered(std::string pipeline, std::string element, std::unique_ptr<GError, GErrorDeleter> err);
	virtual void buffer_high();
	virtual void buffer_low();
	
	GStreamerBase(std::string name);
	
	GStreamerBase(GStreamerBase&& base);
	
	virtual ~GStreamerBase();
	
	void run_loop();
	void stop_loop();
	void make_debug_graph();
};
