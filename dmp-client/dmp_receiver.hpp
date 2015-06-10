#pragma once

#include "gstreamer_base.hpp"

#include <gst/gstelement.h>

#include <cstdint>
#include <string>
#include <memory>

class DmpReceiver : public GStreamerBase
{	
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> source;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> buffer;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> decoder;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> converter;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> resampler;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> volume;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> audiosink;

	std::string radio_name;
	double dvolume;
	
	void cleanup();
public:
	DmpReceiver(std::string gst_dir);
	//DmpReceiver(std::string radio_name, std::string gst_dir);
	
	DmpReceiver(DmpReceiver&&) = default;
	DmpReceiver& operator=(DmpReceiver&&) = default;
	
	virtual void eos_reached() override final;
	
	void setup(std::string name, std::string host, uint16_t port);
	void play();
	void pause();
	void stop();
	void mute(bool mute);
	void set_volume(int volume);
	int get_volume();
	std::string radio_target();
};
