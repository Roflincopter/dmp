#pragma once

#include "gstreamer_base.hpp"

#include <gst/gstelement.h>
#include <gst/gstpad.h>

#include <stdint.h>
#include <string>
#include <memory>

class DmpRadioEndpoint
{
	std::string name;
	uint16_t port;
	
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> bin;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> buffer;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> sink;
	
public:
	DmpRadioEndpoint(std::string name, uint16_t port);
	
	DmpRadioEndpoint(DmpRadioEndpoint&&) = default;
	
	void play();
	
	GstElement* get_sink();
	GstPad* get_sink_pad();
	GstElement* get_bin();
	uint16_t get_port();
};
