#pragma once

#include "gstreamer_base.hpp"
#include "dmp_client_radio_interface.hpp"

#include <cstdint>
#include <string>

class DmpSender : public GStreamerBase
{	
	std::weak_ptr<DmpClientRadioInterface> client;
	std::string radio_name;
	
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> source;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> decoder;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> converter;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> resampler;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> encoder;
//	std::unique_ptr<GstElement, GStreamerEmptyDeleter> rtppay;
	std::unique_ptr<GstElement, GStreamerEmptyDeleter> sink;
	
	bool is_resetting = false;
	
public:
	DmpSender(std::weak_ptr<DmpClientRadioInterface> client, std::string radio_name);
	
	DmpSender(DmpSender&&) = default;
	DmpSender& operator=(DmpSender&&) = default;

	void setup(std::string host, uint16_t port, std::string file);
	
	virtual void eos_reached() override final;
	
	void pause();
	void play();
	void stop();
	void reset();
};
