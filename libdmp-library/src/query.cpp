
#include "query.hpp"

#include "library-entry.hpp"

#include <boost/format.hpp>

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <iostream>

namespace dmp_library {
using namespace boost;
using namespace std;
using std::shared_ptr;

Query::field Query::to_field(string const& x)
{
	if (x == "artist") return field::artist;
	if (x == "title")  return field::title;
	if (x == "album")  return field::album;
	throw std::runtime_error("String: " + x + " did not represent a field");
}

Query::modifier Query::to_modifier(string const& x)
{
	if (x == "contains") return modifier::contains;
	if (x == "is")       return modifier::is;
	throw std::runtime_error("String: " + x + " did not represent a modifier");
}

Atom::Atom(const ast::Atom &ast)
: field(to_field(ast.field))
, modifier(to_modifier(ast.modifier))
, query_string(ast.input)
{}

Atom::Atom(Query::field field, Query::modifier modifier, string query_string)
: field(field)
, modifier(modifier)
, query_string(query_string)
{}

std::string const& Atom::get_field_string(Query::field const& f, LibraryEntry const& e)
{
	switch(f)
	{
		case Query::field::album:  return e.album;
		case Query::field::artist: return e.artist;
		case Query::field::title:  return e.title;
		default: throw runtime_error("This should not happen: __FILE__ : __LINE__##.");
	}
}

boost::regex const Atom::get_regex(Query::modifier const& m)
{
	switch(m)
	{
		case Query::modifier::is:       return regex(query_string);
		case Query::modifier::contains: return regex(str(format(".*%1%.*") % query_string));
		default: throw runtime_error("This should not happen: __FILE__ : __LINE__##.");
	}
}

std::vector<LibraryEntry> Atom::handle_search(const Library::library_t& library)
{
	std::vector<LibraryEntry> ret;
	for(auto&& pair : library)
	{
		auto entry = pair.second.entry;
		std::string const& source = get_field_string(field, entry);
		boost::regex x = get_regex(modifier);

		if(boost::regex_match(source, x))
		{
			ret.emplace_back(entry);
		}
	}
	return ret;
}

And::And(dmp_library::ast::And const& ast)
: lh(apply_visitor(to_query, ast.lh))
, rh(apply_visitor(to_query, ast.rh))
{}

std::vector<LibraryEntry> And::handle_search(const Library::library_t& library)
{
	auto lhv = lh->handle_search(library);
	cout << " and ";
	auto rhv = rh->handle_search(library);

	std::vector<LibraryEntry> ret;
	std::set_intersection(lhv.begin(), lhv.end(), rhv.begin(), rhv.end(), std::back_inserter(ret));
	return ret;
}

Or::Or(const ast::Or& ast)
: lh(apply_visitor(to_query, ast.lh))
, rh(apply_visitor(to_query, ast.rh))
{}

std::vector<LibraryEntry> Or::handle_search(Library::library_t const& library)
{
	auto lhv = lh->handle_search(library);
	cout << " or ";
	auto rhv = rh->handle_search(library);

	std::vector<LibraryEntry> ret;
	std::set_union(lhv.begin(), lhv.end(), rhv.begin(), rhv.end(), std::back_inserter(ret));
	return ret;
}

Not::Not(const ast::Not& ast)
: negated(apply_visitor(to_query, ast.negated))
{}

std::vector<LibraryEntry> Not::handle_search(const Library::library_t& library)
{
	auto x = negated->handle_search(library);

	std::vector<LibraryEntry> original;
	original.reserve(library.size());
	std::transform(library.begin(), library.end(), std::back_inserter(original), [](Library::library_t::value_type const& x) {return x.second.entry;});

	std::vector<LibraryEntry> ret;
	std::set_difference(original.begin(), original.end(), x.begin(), x.end(), std::back_inserter(ret));

	return ret;
}

}
