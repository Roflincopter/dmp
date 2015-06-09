
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

vector<size_t> LibrarySearcher::search(std::shared_ptr<Query> query_obj, LibraryFolder const& folder) {
	return query_obj->handle_search(folder.tracklist);
}

std::map<uint32_t, std::vector<LibraryEntry>> LibrarySearcher::search(std::string query)
{
	auto&& query_obj = parse_query(query);
	
	std::map<uint32_t, std::vector<LibraryEntry>> result;
	for(auto&& entry : library.library) {
		vector<size_t> result_indices = search(query_obj, entry.second);
		vector<LibraryEntry> sub_result;
		for(auto&& index : result_indices) {
			sub_result.push_back(entry.second.tracklist[index]);
		}
		result.emplace(entry.first, sub_result);
	}
	return result;
}

}
