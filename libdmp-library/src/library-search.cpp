
#include "library-search.hpp"

#include <iostream>
#include <boost/regex.hpp>
#include <boost/format.hpp>

namespace dmp_library {
using namespace std;
using namespace boost;

vector<size_t> library_search::search(std::shared_ptr<query> query_obj) {
    return query_obj->handle_search(library);
}

}