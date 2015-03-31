#pragma once

#include "library-entry.hpp"
#include "library-search.hpp"
#include "library.hpp"

#include <dmp_config.hpp>

#include <boost/filesystem/path.hpp>

#include <vector>

namespace dmp_library
{
	const boost::filesystem::path cache_folder = config::get_library_folder_name();

	Library create_library(std::string path, bool use_cache = true, bool create_cache = true);
}
