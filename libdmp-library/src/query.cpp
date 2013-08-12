
#include "query.hpp"

#include <boost/format.hpp>

#include <iostream>

namespace dmp_library {
using namespace boost;
using namespace std;
using std::shared_ptr;

query::field query::to_field(string const& x)
{
    if (x == "artist") return field::artist;
    if (x == "title")  return field::title;
    if (x == "album")  return field::album;
}

query::modifier query::to_modifier(string const& x)
{
    if (x == "contains") return modifier::contains;
    if (x == "is")       return modifier::is;
}

atom::atom(ast_atom const& ast)
: field(to_field(ast.field))
, modifier(to_modifier(ast.modifier))
, query_string(ast.input)
{}

atom::atom(query::field field, query::modifier modifier, string query_string)
: field(field)
, modifier(modifier)
, query_string(query_string)
{}

std::string const& atom::get_field_string(query::field const& f, library_entry const& e)
{
    switch(f)
    {
        case query::field::album:  return e.album;
        case query::field::artist: return e.artist;
        case query::field::title:  return e.title;
        default: throw runtime_error("This should not happen: __FILE__ : __LINE__##.");
    }
}

boost::regex const atom::get_regex(query::modifier const& m)
{
    switch(m)
    {
        case query::modifier::is:       return regex(query_string);
        case query::modifier::contains: return regex(str(format(".*%1%.*") % query_string));
        default: throw runtime_error("This should not happen: __FILE__ : __LINE__##.");
    }
}

std::vector<size_t> atom::handle_search(vector<library_entry> const& library)
{
    for(library_entry const& entry : library)
    {
        std::string const& source = get_field_string(field, entry);
    }
    cout << "atom";
    return std::vector<size_t>();
}

_and::_and(dmp_library::ast_and const& ast)
: lh(apply_visitor(to_query, ast.lh))
, rh(apply_visitor(to_query, ast.rh))
{}

_and::_and(std::shared_ptr<query> lh, shared_ptr<query> rh)
: lh(lh)
, rh(rh)
{}

std::vector<size_t> _and::handle_search(vector<library_entry> const& library)
{
    lh->handle_search(library);
    cout << " and ";
    rh->handle_search(library);
    return std::vector<size_t>();
}

_or::_or(dmp_library::ast_or const& ast)
: lh(apply_visitor(to_query, ast.lh))
, rh(apply_visitor(to_query, ast.rh))
{}

_or::_or(std::shared_ptr<query> lh, shared_ptr<query> rh)
: lh(lh)
, rh(rh)
{}

std::vector<size_t> _or::handle_search(vector<library_entry> const& library)
{
    lh->handle_search(library);
    cout << " or ";
    rh->handle_search(library);
    return std::vector<size_t>();
}

_not::_not(dmp_library::ast_not const& ast)
: negated(apply_visitor(to_query, ast.negated))
{}

_not::_not(shared_ptr<query> arg)
: negated(negated)
{}

std::vector<size_t> _not::handle_search(vector<library_entry> const& library)
{
    cout << " not ";
    negated->handle_search(library);
    return std::vector<size_t>();
}

}