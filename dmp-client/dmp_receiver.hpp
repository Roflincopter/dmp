#pragma once

#include <gst/gst.h>

#include <cstdint>
#include <string>

class DmpReceiver
{
	GMainLoop* loop;
	GstElement* pipeline;
	GstElement* source;
	GstElement* decoder;
	GstElement* converter;
	GstElement* resampler;
	GstElement* audiosink;
	GstBus *bus;

	void cleanup();
public:
	DmpReceiver();
	void stop();
	void connect(std::string host, uint16_t port);
};

