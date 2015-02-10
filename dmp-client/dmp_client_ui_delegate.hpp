#pragma once

#include "message.hpp"

#include <string>

struct DmpClientUiDelegate
{
	virtual void client_stopped(){}
	virtual void add_radio_succes(message::AddRadioResponse){}
	virtual void add_radio_failed(message::AddRadioResponse){}

	virtual void set_play_paused_state(bool state){}

	virtual void volume_changed(int volume){};
	
	virtual void login_succeeded(){}
	virtual void login_failed(std::string reason){}
	
	virtual void register_succeeded(){}
	virtual void register_failed(std::string reason){}

	virtual ~DmpClientUiDelegate(){}
};
