
#include "gstreamer_base.hpp"

#include "time_util.hpp"
#include "debug_macros.hpp"

#include <glib/gerror.h>
#include <glib/gmain.h>
#include <glib/gmem.h>
#include <glib/gtypes.h>
#include <gst/gstbin.h>
#include <gst/gstbus.h>
#include <gst/gstclock.h>
#include <gst/gstdebugutils.h>
#include <gst/gstelement.h>
#include <gst/gstmessage.h>
#include <gst/gstobject.h>
#include <gst/gstpad.h>
#include <gst/gstpipeline.h>
#include <gst/gstversion.h>

#if defined( _WIN32 ) || defined( _WIN64 )
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#include <algorithm>
#include <limits>
#include <iostream>
#include <cstdlib>

void GStreamerBase::eos_reached()
{
	std::cerr << "End of stream Reached." << std::endl;
}

void GStreamerBase::error_encountered(std::string pipeline, std::string element, std::unique_ptr<GError, GErrorDeleter> err)
{
	std::string message = std::string(err->message);
	std::cerr << pipeline << ":" << element << " message: " << message << std::endl;

	if(getenv("GST_DEBUG_DUMP_DOT_DIR"))
	{
		std::string filename = make_debug_graph(message);
		std::cout << "debug_dot_file_created at: " << filename << std::endl;
	}
}

void GStreamerBase::state_changed(std::string element, GstState old_, GstState new_, GstState pending)
{
	std::cout << "State change: " << name << ":" << element << " from: " << gst_state_to_string(old_) << " to: " << gst_state_to_string(new_) << " pending?: "  << gst_state_to_string(pending) << std::endl;
}

GStreamerBase::GStreamerBase(std::string name, std::string gst_dir)
: GStreamerInit(gst_dir)
, gstreamer_mutex(new std::recursive_mutex())
, name(name)
, pipeline(gst_pipeline_new(name.c_str()))
, bus(gst_pipeline_get_bus(GST_PIPELINE(pipeline.get())))
{
	gst_bus_set_sync_handler(bus.get(), &bus_call, this, nullptr);
}

GStreamerBase::GStreamerBase(GStreamerBase&& base)
: GStreamerInit(std::move(base))
, gstreamer_mutex(std::move(base.gstreamer_mutex))
, name(std::move(base.name))
, pipeline(std::move(base.pipeline))
, bus(std::move(base.bus))
{
	base.name = "<INVALID>";
	base.pipeline.reset();
	base.bus.reset();
	gst_bus_set_sync_handler(bus.get(), nullptr, nullptr, nullptr);
	gst_bus_set_sync_handler(bus.get(), &bus_call, this, nullptr);
}

GStreamerBase::~GStreamerBase()
{
	shutdown();
}

void GStreamerBase::shutdown()
{
	if(bus) {
		gst_bus_set_sync_handler(bus.get(), nullptr, nullptr, nullptr);
	}
	if(pipeline) {
		gst_element_set_state(pipeline.get(), GST_STATE_NULL);
	}
}

std::string GStreamerBase::make_debug_graph(std::string prefix)
{
	std::string filename = get_current_time() + "_" + prefix + (prefix.empty() ? "" : "_") + name;
	GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline.get()), GST_DEBUG_GRAPH_SHOW_ALL, filename.c_str());
	return filename;
}

GstState GStreamerBase::wait_for_state_change(GstClockTime timeout)
{
	GstState state;
	GstState pending;
	if(gst_element_get_state(pipeline.get(), &state, &pending, timeout) == GST_STATE_CHANGE_ASYNC) {
		DEBUG_COUT << "State change of " << name << " to " << gst_state_to_string(pending) << " did not complete in time. Making debug dot." << std::endl;
		make_debug_graph("StateChangeTimeout");
	}
	return state;
}

GstState GStreamerBase::target_state()
{
	GstState state;
	GstState pending;
	gst_element_get_state(pipeline.get(), &state, &pending, 0);
	return pending == GST_STATE_VOID_PENDING ? state : pending;
}

GstBusSyncReply GStreamerBase::bus_call (GstBus*, GstMessage* msg, gpointer data)
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
		
		gst_message_parse_state_changed(msg, &old_, &new_, &pending);
		
		char* x = gst_object_get_name(GST_MESSAGE_SRC(msg));
		std::string element(x);
		g_free(x);
		
		base->state_changed(element, old_, new_, pending);
		
		break;
	}

	case GST_MESSAGE_ERROR: {
		gchar  *debug_ptr;
		GError *temp_error_ptr;
	
		gst_message_parse_error (msg, &temp_error_ptr, &debug_ptr);
		
		std::unique_ptr<GError, GErrorDeleter> error_ptr(temp_error_ptr);
		std::string debug(debug_ptr ? debug_ptr : "");
		
		g_free (debug_ptr);
		
		char* x = gst_object_get_name(GST_MESSAGE_SRC(msg));
		std::string element(x);
		g_free(x);
		
		base->error_encountered(base->name, element, std::move(error_ptr));
		break;
	}
	
	case GST_MESSAGE_BUFFERING:
	case GST_MESSAGE_UNKNOWN:
	case GST_MESSAGE_WARNING:
	case GST_MESSAGE_INFO:
	case GST_MESSAGE_TAG:
	case GST_MESSAGE_STATE_DIRTY:
	case GST_MESSAGE_STEP_DONE:
	case GST_MESSAGE_CLOCK_PROVIDE:
	case GST_MESSAGE_CLOCK_LOST:
	case GST_MESSAGE_NEW_CLOCK:
	case GST_MESSAGE_STRUCTURE_CHANGE:
	case GST_MESSAGE_STREAM_STATUS:
	case GST_MESSAGE_APPLICATION:
	case GST_MESSAGE_ELEMENT:
	case GST_MESSAGE_SEGMENT_START:
	case GST_MESSAGE_SEGMENT_DONE:
	case GST_MESSAGE_DURATION_CHANGED:
	case GST_MESSAGE_LATENCY:
	case GST_MESSAGE_ASYNC_START:
	case GST_MESSAGE_ASYNC_DONE:
	case GST_MESSAGE_REQUEST_STATE:
	case GST_MESSAGE_STEP_START:
	case GST_MESSAGE_QOS:
	case GST_MESSAGE_PROGRESS:
	case GST_MESSAGE_TOC:
	case GST_MESSAGE_RESET_TIME:
	case GST_MESSAGE_STREAM_START:
	case GST_MESSAGE_NEED_CONTEXT:
	case GST_MESSAGE_HAVE_CONTEXT:
	case GST_MESSAGE_ANY:
#if GST_VERSION_MAJOR >= 1 && GST_VERSION_MINOR >= 4
	case GST_MESSAGE_EXTENDED:
	case GST_MESSAGE_DEVICE_ADDED:
	case GST_MESSAGE_DEVICE_REMOVED:
#endif
	default:
		break;
	}

	// Every message is handled here. Passing it on would queue it on the bus,
	// where nothing ever pops it, so it would be leaked.
	return GST_BUS_DROP;
}

void on_pad_added (GstElement*, GstPad* pad, gpointer data)
{
	GstElement* next = static_cast<GstElement*>(data);

	// decodebin can expose non-audio pads (e.g. embedded cover art); only the
	// first audio pad is linked.
	GstCaps* caps = gst_pad_get_current_caps(pad);
	if(!caps) {
		caps = gst_pad_query_caps(pad, nullptr);
	}
	bool is_audio = false;
	if(caps && gst_caps_get_size(caps) > 0) {
		std::string media_type(gst_structure_get_name(gst_caps_get_structure(caps, 0)));
		is_audio = media_type.rfind("audio/", 0) == 0;
	}
	if(caps) {
		gst_caps_unref(caps);
	}
	if(!is_audio) {
		return;
	}

	GstPad* sinkpad = gst_element_get_static_pad(next, "sink");
	if(!gst_pad_is_linked(sinkpad)) {
		if(gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) {
			std::cerr << "Failed to link decoded audio pad" << std::endl;
		}
	}
	gst_object_unref(sinkpad);
}

std::string gst_state_to_string(GstState x)
{
	if(x == GST_STATE_VOID_PENDING) return "GST_STATE_VOID_PENDING";
	if(x == GST_STATE_NULL) return "GST_STATE_NULL";
	if(x == GST_STATE_READY) return "GST_STATE_READY";
	if(x == GST_STATE_PAUSED) return "GST_STATE_PAUSED";
	if(x == GST_STATE_PLAYING) return "GST_STATE_PLAYING";
	return "<UNKNOWN>";
}

GStreamerInit::GStreamerInit(std::string gst_dir) {
#if defined( _WIN32 ) || defined( _WIN64 )
	char buffer[512];
	GetModuleFileName(nullptr, buffer, sizeof(buffer));
	boost::filesystem::path executable_path(buffer);
	boost::filesystem::path plugin_path = executable_path.parent_path() / boost::filesystem::path("plugins\\gstreamer");
	
	DEBUG_COUT << "Setting plugin path to: " << plugin_path.string() << std::endl;
	
	g_setenv("GST_PLUGIN_PATH_1_0", plugin_path.string().c_str(), false);
#endif
	if(!gst_is_initialized()) {
		g_setenv("GST_DEBUG_DUMP_DOT_DIR", gst_dir.c_str(), false);
		gst_init(nullptr, nullptr);
	}
}
