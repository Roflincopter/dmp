#pragma once

#include "debug_macros.hpp"

#include <gst/gst.h>

#include <glib/genviron.h>
#include <glib/gerror.h>
#include <glib/gmain.h>
#include <glib/gtypes.h>
#include <gst/gstbin.h>
#include <gst/gstbus.h>
#include <gst/gstelement.h>
#include <gst/gstmessage.h>
#include <gst/gstobject.h>
#include <gst/gstpad.h>
#include <gst/gstpadtemplate.h>

#include <string>
#include <memory>
#include <mutex>

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
	
	GStreamerInit(std::string gst_dir);

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
	virtual void state_changed(std::string element, GstState old_, GstState new_, GstState pending);
	
	GStreamerBase(std::string name, std::string gst_dir);
	
	GStreamerBase(GStreamerBase&& base);
	
	virtual ~GStreamerBase();
	
	void run_loop();
	void stop_loop();
	std::string make_debug_graph(std::string prefix = "");
	GstState wait_for_state_change();
};
