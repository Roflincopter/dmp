
#include "library-search.hpp"

#include "library-entry.hpp"
#include "library.hpp"
#include "query-parser.hpp"
#include "query.hpp"

#include <utility>

namespace dmp_library {
using namespace std;
using namespace boost;

LibrarySearcher::LibrarySearcher(const Library& library)
: library(library)
{}

std::vector<LibraryEntry> LibrarySearcher::search(std::shared_ptr<Query> query_obj, Library const& library) {
	return query_obj->handle_search(library.library);
}

std::vector<LibraryEntry> LibrarySearcher::search(std::string query)
{
	auto&& query_obj = parse_query(query);
	return search(query_obj, library.library);
}

}
