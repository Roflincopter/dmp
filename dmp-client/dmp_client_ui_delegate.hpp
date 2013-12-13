#pragma once

#include "message.hpp"

#include <string>

struct DmpClientUiDelegate
{
    virtual void query_parse_error(std::string error){}
    virtual void search_results(message::SearchResponse search_response){}

    virtual ~DmpClientUiDelegate(){}
};
