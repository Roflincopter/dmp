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

struct GStreamerObjectDeleter {
	void operator()(GstElement* ptr) {
		// An element must be in the NULL state before its last reference is
		// dropped, otherwise its streaming threads are left running.
		gst_element_set_state(ptr, GST_STATE_NULL);
		gst_object_unref(ptr);
	}
	
	void operator()(GstBus* ptr) {
		gst_object_unref(ptr);
	}
	
	void operator()(GstBin* ptr) {
		gst_element_set_state(GST_ELEMENT(ptr), GST_STATE_NULL);
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
		// gst_element_request_pad returned a reference of our own.
		gst_object_unref(ptr);
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

	static GstBusSyncReply bus_call(GstBus* bus, GstMessage* msg, gpointer data);

	// Stops the pipeline and detaches the bus handler. Derived classes must
	// call this first thing in their destructor: the bus handler calls their
	// virtual functions from GStreamer's streaming threads.
	void shutdown();

public:
	std::string name;
	
	std::unique_ptr<GstElement, GStreamerObjectDeleter> pipeline;
	std::unique_ptr<GstBus, GStreamerObjectDeleter> bus;
	
	// These are called from the bus sync handler, i.e. on whatever GStreamer
	// thread posted the message. Implementations must not block and must not
	// change the state of this pipeline directly; post work to the
	// application's own thread instead.
	virtual void eos_reached();
	virtual void error_encountered(std::string pipeline, std::string element, std::unique_ptr<GError, GErrorDeleter> err);
	virtual void state_changed(std::string element, GstState old_, GstState new_, GstState pending);
	
	GStreamerBase(std::string name, std::string gst_dir);
	GStreamerBase(GStreamerBase&& base);
	virtual ~GStreamerBase();
	
	std::string make_debug_graph(std::string prefix = "");

	// Waits (at most timeout) for a pending asynchronous state change to finish
	// and returns the resulting state.
	GstState wait_for_state_change(GstClockTime timeout = 5 * GST_SECOND);

	// The state the pipeline is in or is heading to, without blocking.
	GstState target_state();
};
