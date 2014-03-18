#pragma once

#include "gstreamer_base.hpp"

#include <cstdint>
#include <string>

class DmpSender : public GStreamerBase
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
	virtual ~DmpSender() = default;
	
	virtual void eos_reached() override final;

	void run(std::string host, uint16_t port, std::string file);
};
