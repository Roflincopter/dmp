

#include "query.hpp"

namespace dmp_library {
using namespace boost;

query::field query::to_field(std::string const& x) 
{
	if (x == "artist") return field::artist;
	if (x == "title")  return field::title;
	if (x == "album")  return field::album;
}

query::modifier query::to_modifier(std::string const& x)
{
	if (x == "contains") return modifier::contains;
	if (x == "is")       return modifier::is;
}

atom::atom(query::field field, query::modifier modifier, std::string query_string)
: field(field)
, modifier(modifier)
, query_string(query_string)
{}

std::vector<size_t> atom::handle_search(std::vector<library_entry> const& library)
{
	#warning unimplemented
}

_and::_and(dmp_library::ast_and const& ast)
: lh(apply_visitor(to_query, ast.lh))
, rh(apply_visitor(to_query, ast.rh))
{}

_and::_and(std::shared_ptr<query> lh, std::shared_ptr<query> rh)
: lh(lh)
, rh(rh)
{}

std::vector<size_t> _and::handle_search(std::vector<library_entry> const& library)
{
	#warning unimplemented
}

_or::_or(dmp_library::ast_or const& ast)
: lh(apply_visitor(to_query, ast.lh))
, rh(apply_visitor(to_query, ast.rh))
{}

_or::_or(std::shared_ptr<query> lh, std::shared_ptr<query> rh)
: lh(lh)
, rh(rh)
{}

std::vector<size_t> _or::handle_search(std::vector<library_entry> const& library)
{
	#warning unimplemented
}

_not::_not(dmp_library::ast_not const& ast)
: negated(apply_visitor(to_query, ast.negated))
{}

_not::_not(std::shared_ptr<query> arg)
: negated(negated)
{}

std::vector<size_t> _not::handle_search(std::vector<library_entry> const& library)
{
	#warning unimplemented
}

}