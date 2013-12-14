#pragma once

#include "query.hpp"

#include <memory>

namespace dmp_library {

std::shared_ptr<Query> parse_query(std::string const& str);

struct ParseError{
    std::string expected;
    size_t pivot;

    ParseError()
    : expected()
    , pivot()
    {}

    std::string what() const;
};

}
