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

#if defined( _WIN32 ) || defined( _WIN64 )
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

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
	
	GStreamerInit(std::string gst_dir) {
#if defined( _WIN32 ) || defined( _WIN64 )
		char buffer[512];
		GetModuleFileName(nullptr, buffer, sizeof(buffer));
		boost::filesystem::path executable_path(buffer);
		boost::filesystem::path plugin_path = executable_path.stem() / boost::filesystem::path("plugins/gstreamer");

		DEBUG_COUT << "Setting plugin path to: " << plugin_path.string() << std::endl;

		g_setenv("GST_PLUGIN_PATH_1_0", plugin_path.string().c_str(), false);
#endif
		if(!gst_is_initialized()) {
			g_setenv("GST_DEBUG_DUMP_DOT_DIR", gst_dir.c_str(), false);
			gst_init(nullptr, nullptr);
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
	
	GStreamerBase(std::string name, std::string gst_dir);
	
	GStreamerBase(GStreamerBase&& base);
	
	virtual ~GStreamerBase();
	
	void run_loop();
	void stop_loop();
	std::string make_debug_graph(std::string prefix = "");
	GstState wait_for_state_change();
};
