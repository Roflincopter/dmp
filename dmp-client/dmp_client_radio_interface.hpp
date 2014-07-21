#pragma once

#include "message.hpp"

struct DmpClientRadioInterface
{
	virtual void forward_radio_action(message::RadioAction re) = 0;
};
