#pragma once

#include "message.hpp"
#include "load_info.hpp"

#include <string>

struct DmpClientUiDelegate
{
	virtual void client_stopped(){}
	virtual void add_radio_succes(message::AddRadioResponse){}
	virtual void add_radio_failed(message::AddRadioResponse){}

	virtual void set_play_paused_state(bool __attribute__((unused)) state){}

	virtual void volume_changed(int __attribute__((unused)) volume){};
	
	virtual void login_succeeded(){}
	virtual void login_failed(std::string __attribute__((unused)) reason){}
	
	virtual void register_succeeded(){}
	virtual void register_failed(std::string __attribute__((unused)) reason){}
	
	virtual void library_load_start(){}
	virtual void library_load_info(std::shared_ptr<dmp_library::LoadInfo> __attribute__((unused)) info){}
	virtual void library_load_end(){}

	virtual ~DmpClientUiDelegate(){}
};
