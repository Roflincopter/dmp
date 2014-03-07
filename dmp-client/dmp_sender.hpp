#pragma once

#include <gst/gst.h>

#include <cstdint>
#include <string>

class DmpSender
{	
	GMainLoop* loop;
	
	GstElement* pipeline;
	GstElement* source;
	GstElement* decoder;
	GstElement* encoder;
	GstElement* sink;
	
	GstBus *bus;
	
public:
	DmpSender();

	void run(std::string host, uint16_t port, std::string file);
};
