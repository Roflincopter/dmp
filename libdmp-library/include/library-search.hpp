#pragma once

#include "library-entry.hpp"
#include "query-parser.hpp"

#include <vector>
#include <algorithm>
#include <memory>

namespace dmp_library {

class library_search
{
    std::vector<library_entry> const& library;

public:

    library_search(std::vector<library_entry> const& library)
    : library(library)
    {}

    std::vector<size_t> search(std::shared_ptr<query> query_obj);

};

}
