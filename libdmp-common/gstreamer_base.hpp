#pragma once

#include <gst/gst.h>

#include <string>
#include <memory>
#include <mutex>
#include <vector>

#define CHECK_GSTREAMER_COMPONENT(x) if(!x) std::cout << "Failed to create: " << #x << " element variable." << std::endl

void on_pad_added(_GstElement* element, _GstPad* pad, void* data);
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

struct GStreamerInit {
	
	GStreamerInit() {
		if(!gst_is_initialized()) {
			char arg0[] = "";
			char arg1[] = "--gst-plugin-path=plugins/gstreamer";
			char* argv[] = { &arg0[0], &arg1[0], NULL };
			int argc = (int)(sizeof(argv) / sizeof(argv[0])) - 1;
			char** ptr = &argv[0];
			
			gst_init(&argc, &ptr);
		}
	}

};

class GStreamerBase : GStreamerInit {
protected:
	std::unique_ptr<std::recursive_mutex> gstreamer_mutex;

	static gboolean bus_call (GstBus* bus, GstMessage* msg, gpointer data);

public:
	bool buffering;
	
	std::string name;
	
	std::unique_ptr<GMainLoop, GMainLoopDeleter> loop;
	
	std::unique_ptr<GstElement, GStreamerObjectDeleter> pipeline;
	std::unique_ptr<GstBus, GStreamerObjectDeleter> bus;
	
	guint gst_bus_watch_id;
	
	virtual void eos_reached();
	virtual void error_encountered(std::string pipeline, std::string element, std::unique_ptr<GError, GErrorDeleter> err);
	virtual void buffer_high(GstElement* src);
	virtual void buffer_low(GstElement* src);
	
	GStreamerBase(std::string name);
	
	GStreamerBase(GStreamerBase&& base);
	
	virtual ~GStreamerBase();
	
	void run_loop();
	void stop_loop();
	std::string make_debug_graph(std::string prefix = "");
	void wait_for_state_change();
};
