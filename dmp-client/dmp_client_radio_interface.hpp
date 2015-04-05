#pragma once

#include "message.hpp"

struct DmpClientRadioInterface
{
	virtual void forward_radio_action(message::RadioAction ra) = 0;
	virtual void forward_radio_event(message::SenderEvent re) = 0;

	virtual ~DmpClientRadioInterface(){}
};
