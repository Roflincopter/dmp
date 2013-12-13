#pragma once

#include "dmp_client_interface.hpp"

struct DmpClientUiControllerInterface {

    virtual void set_client(std::shared_ptr<DmpClientInterface> new_client) = 0;
};
