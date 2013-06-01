

#include "query.hpp"

namespace dmp_library {

atom::atom(query::field field, query::modifier modifier, std::string query_string)
: field(field)
, modifier(modifier)
, query_string(query_string)
{}

std::vector<size_t> atom::handle_search(std::vector<library_entry> const& library)
{
	#warning unimplemented
}

_and::_and(std::shared_ptr<query> lh, std::shared_ptr<query> rh)
: lh(lh)
, rh(rh)
{}

std::vector<size_t> _and::handle_search(std::vector<library_entry> const& library)
{
	#warning unimplemented
}

_or::_or(std::shared_ptr<query> lh, std::shared_ptr<query> rh)
: lh(lh)
, rh(rh)
{}

std::vector<size_t> _or::handle_search(std::vector<library_entry> const& library)
{
	#warning unimplemented
}

_not::_not(std::shared_ptr<query> arg)
: arg(arg)
{}

std::vector<size_t> _not::handle_search(std::vector<library_entry> const& library)
{
	#warning unimplemented
}

}