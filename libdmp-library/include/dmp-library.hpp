#pragma once

#include "library-entry.hpp"
#include "library-search.hpp"
#include "library.hpp"

#include <vector>

namespace dmp_library
{

	const std::string cache_file = ".dmp_library";

	Library create_library(std::string path, bool use_cache = true, bool create_cache = true);
}
