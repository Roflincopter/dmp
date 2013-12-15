#pragma once

#include "message_callbacks.hpp"
#include "dmp_client_ui_delegate.hpp"

struct DmpClientInterface {

	virtual void search(std::string str) = 0;
	virtual void index(std::string str) = 0;
	virtual void stop() = 0;
	virtual void send_bye() = 0;
	virtual void run() = 0;

	virtual void add_delegate(std::weak_ptr<DmpClientUiDelegate> delegate) = 0;

	virtual ~DmpClientInterface(){}
};
