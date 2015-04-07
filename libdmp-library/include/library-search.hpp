#pragma once

#include "library-entry.hpp"
#include "query-parser.hpp"
#include "library.hpp"

#include <vector>
#include <algorithm>
#include <memory>

namespace dmp_library {

class LibrarySearcher
{
	Library const& library;

public:

	LibrarySearcher(Library const& library);

	LibrarySearcher(LibrarySearcher const&) = delete;
	LibrarySearcher(LibrarySearcher&&) = delete;

	std::vector<size_t> search(std::shared_ptr<Query> query_obj, LibraryFolder const& folder);
	std::map<uint32_t, std::vector<LibraryEntry>> search(std::string query);

};

}
