
#include "library-search.hpp"

#include <iostream>
#include <boost/regex.hpp>
#include <boost/format.hpp>

namespace dmp_library {
using namespace std;
using namespace boost;

vector<size_t> LibrarySearcher::search(std::shared_ptr<Query> query_obj) {
    return query_obj->handle_search(library);
}

}
