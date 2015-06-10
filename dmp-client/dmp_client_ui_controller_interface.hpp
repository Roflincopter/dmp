#pragma once

#include <memory>

struct DmpClientInterface;

struct DmpClientUiControllerInterface {

	virtual void set_client(std::shared_ptr<DmpClientInterface> new_client) = 0;

	virtual ~DmpClientUiControllerInterface(){}
};
