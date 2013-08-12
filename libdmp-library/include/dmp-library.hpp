#pragma once

#include "library-entry.hpp"
#include "library-search.hpp"

#include <vector>

namespace dmp_library
{

    std::vector<library_entry> read_path(std::string path);

}