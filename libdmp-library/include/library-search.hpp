#pragma once

#include <stddef.h>
#include <stdint.h>
#include <map>
#include <string>
#include <vector>
#include <memory>

namespace dmp_library {
	struct Library;
	struct LibraryEntry;
	struct LibraryFolder;
	struct Query;
}

namespace dmp_library {

class LibrarySearcher
{
	Library const& library;

public:

	LibrarySearcher(Library const& library);

	LibrarySearcher(LibrarySearcher const&) = delete;
	LibrarySearcher(LibrarySearcher&&) = delete;

	std::vector<std::pair<std::size_t, LibraryEntry>> search(std::shared_ptr<Query> query_obj, Library const& library);
	std::vector<std::pair<std::size_t, LibraryEntry>> search(std::string query);

};

}
