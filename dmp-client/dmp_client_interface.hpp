#pragma once

#include "message_callbacks.hpp"

struct DmpClientInterface {

    virtual void search(std::string str) = 0;
    virtual void index(std::string str) = 0;
    virtual message::DmpCallbacks& get_callbacks() = 0;
    virtual void stop() = 0;
    virtual void send_bye() = 0;
    virtual void run() = 0;

    virtual ~DmpClientInterface(){}
};