#pragma once

#include "library-entry.hpp"
#include "query-parser.hpp"

#include <vector>
#include <algorithm>
#include <memory>

namespace dmp_library {

class LibrarySearcher
{
	std::vector<LibraryEntry> const& library;

public:

	LibrarySearcher(std::vector<LibraryEntry> const& library)
	: library(library)
	{}

	std::vector<size_t> search(std::shared_ptr<Query> query_obj);
	std::vector<LibraryEntry> search(std::string query);

};

}
