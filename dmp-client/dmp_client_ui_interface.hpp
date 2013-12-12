#pragma once

#include <string>

struct UiInterface
{
    virtual void query_parse_error(std::string err) const = 0;

    virtual ~UiInterface(){}
};
