#pragma once

#include "gstreamer_base.hpp"

#include <cstdint>
#include <string>

class DmpReceiver : public GStreamerBase
{	
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> source;
//	std::unique_ptr<GstElement, GStreamerEmptyDeleter> rtpdepay;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> decoder;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> converter;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> resampler;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> audiosink;

	void cleanup();
public:
	DmpReceiver();
	
	DmpReceiver(DmpReceiver&&) = default;
	DmpReceiver& operator=(DmpReceiver&&) = default;
	
	void setup(std::string host, uint16_t port);
	void play();
};

