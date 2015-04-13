
#include "gstreamer_base.hpp"

#include "time_util.hpp"
#include "debug_macros.hpp"

#include <iostream>
#include <cassert>
#include <chrono>
#include <sstream>
#include <iomanip>
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

void GStreamerBase::buffer_high(GstElement*)
{
	gst_element_set_state(pipeline.get(), GST_STATE_PLAYING);
}

void GStreamerBase::buffer_low(GstElement*)
{
	gst_element_set_state(pipeline.get(), GST_STATE_PAUSED);
}

GStreamerBase::GStreamerBase(GStreamerBase&& base)
: GStreamerInit()
, gstreamer_mutex(std::move(base.gstreamer_mutex))
, buffering(false)
, name(std::move(base.name))
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
: GStreamerInit()
, gstreamer_mutex(new std::recursive_mutex())
, buffering(false)
, name(name)
, loop(g_main_loop_new(nullptr, false))
, pipeline(gst_pipeline_new(name.c_str()))
, bus(gst_pipeline_get_bus(GST_PIPELINE(pipeline.get())))
, gst_bus_watch_id(gst_bus_add_watch(bus.get(), bus_call, this))
{
}

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

std::string GStreamerBase::make_debug_graph(std::string prefix)
{
	std::string filename = get_current_time() + "_" + prefix + (prefix.empty() ? "" : "_") + name;
	GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN(pipeline.get()), GST_DEBUG_GRAPH_SHOW_ALL, filename.c_str());
	return filename;
}

void GStreamerBase::wait_for_state_change()
{
	GstState state;
	GstState pending;
	if(!gst_element_get_state(pipeline.get(), &state, &pending, std::numeric_limits<GstClockTime>::max())) {
		DEBUG_COUT << "State change did not complete within 5s. Making debug dot." << std::endl;
		make_debug_graph("StateChangeTimeout");
	}
}

gboolean GStreamerBase::bus_call (GstBus*, GstMessage* msg, gpointer data)
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
		
		//std::cout << "State change: " << base->name << ":" << element << " from: " << gst_state_to_string(old_) << " to: " << gst_state_to_string(new_) << " pending?: "  << gst_state_to_string(pending) << std::endl;
		break;
	}

	case GST_MESSAGE_ERROR: {
		gchar  *debug_ptr;
		GError *temp_error_ptr;
	
		gst_message_parse_error (msg, &temp_error_ptr, &debug_ptr);
		
		std::unique_ptr<GError, GErrorDeleter> error_ptr(temp_error_ptr);
		std::string debug(debug_ptr);
		
		g_free (debug_ptr);
		
		char* x = gst_object_get_name(GST_MESSAGE_SRC(msg));
		std::string element(x);
		g_free(x);
		
		base->error_encountered(base->name, element, std::move(error_ptr));
		break;
	}
	
	case GST_MESSAGE_BUFFERING: {
		int percent;
		
		gst_message_parse_buffering (msg, &percent);
		
		if(percent == 100) {
			base->buffering = false;
			base->buffer_high(GST_ELEMENT(GST_MESSAGE_SRC(msg)));
			//base->gstreamer_mutex->unlock();
		} else {
			
			GstState state;
			GstState pending;
			GstClockTime timeout = 0;
			
			gst_element_get_state(base->pipeline.get(), &state, &pending, timeout);
			
			if (!base->buffering && percent < 10 && state == GST_STATE_PLAYING) {
				//base->gstreamer_mutex->lock();
				base->buffer_low(GST_ELEMENT(GST_MESSAGE_SRC(msg)));
			}
			base->buffering = true;
		}
		break;
	}
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

	return true;
}

void on_pad_added (GstElement* __attribute__((unused)) element, GstPad* pad, gpointer data)
{
	GstPad *sinkpad;
	GstElement *decoder = static_cast<GstElement*>(data);

	sinkpad = gst_element_get_static_pad (decoder, "sink");
	gst_pad_link (pad, sinkpad);
	gst_object_unref (sinkpad);
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


