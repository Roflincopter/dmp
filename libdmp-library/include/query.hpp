#pragma once

#include <library-entry.hpp>
#include <library.hpp>

#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/regex/v4/regex_fwd.hpp>
#include <boost/variant.hpp>
#include <boost/regex.hpp>

#include <stddef.h>
#include <memory>
#include <vector>
#include <iostream>
#include <string>
#include <type_traits>

namespace boost { template <typename T> class recursive_wrapper; }

namespace dmp_library {

struct Query;
struct Atom;
struct Not;
struct And;
struct Or;

struct PrintVisitor : public boost::static_visitor<std::ostream&>
{
	std::ostream& os;

	PrintVisitor(std::ostream& os)
	: os(os)
	{}

	template <typename T>
	std::ostream& operator()(T x)
	{
		return os << x;
	}
};

namespace ast
{

struct And;
struct Or;
struct Not;
struct Atom;
struct Nest;

typedef boost::variant<ast::Atom, boost::recursive_wrapper<ast::Or>, boost::recursive_wrapper<ast::And>, boost::recursive_wrapper<ast::Not>, boost::recursive_wrapper<ast::Nest>> AstQueryType;

struct Atom {
	std::string field;
	std::string modifier;
	std::string input;

	typedef ::dmp_library::Atom QueryType;

	friend std::ostream& operator<<(std::ostream& os, Atom const& atom)
	{
		std::cout << atom.field << " " << atom.modifier << " " << atom.input;
		return os;
	}
};

struct Not {
	AstQueryType negated;

	typedef ::dmp_library::Not QueryType;

	friend std::ostream& operator<<(std::ostream& os, Not const& _not)
	{
		os << " not: ";
		PrintVisitor p(os);
		boost::apply_visitor(p, _not.negated);
		return os;
	}
};

struct Or {
	AstQueryType lh;
	AstQueryType rh;

	typedef ::dmp_library::Or QueryType;

	friend std::ostream& operator<<(std::ostream& os, Or const& _or)
	{
		PrintVisitor p(os);
		os << " :or: ";

		os << std::endl << "\t";
		boost::apply_visitor(p, _or.lh);

		os << std::endl << "\t";
		boost::apply_visitor(p, _or.rh);
		return os;
	}
};

struct And {
	AstQueryType lh;
	AstQueryType rh;

	typedef ::dmp_library::And QueryType;

	friend std::ostream& operator<<(std::ostream& os, And const& _and)
	{
		PrintVisitor p(os);
		boost::apply_visitor(p, _and.lh);
		os << " :and: ";
		boost::apply_visitor(p, _and.rh);
		return os;
	}
};

struct Nest {
	AstQueryType arg;

	friend std::ostream& operator<<(std::ostream& os, ast::Nest const& nest)
	{
		os << " (";
		PrintVisitor p(os);
		boost::apply_visitor(p, nest.arg);
		os << ") ";
		return os;
	}
};

template <typename T>
struct rotate_visitor : public boost::static_visitor<T>
{
	T const& parent;

	rotate_visitor(T const& parent_)
	:parent(parent_)
	{}

	template <typename V>
	T rotate(std::false_type, V const&)
	{
		return parent;
	}

	T rotate(std::true_type, T const& child)
	{
		T new_parent;
		T new_child;

		new_child.lh = parent.lh;
		new_child.rh = child.lh;
		new_parent.lh = new_child;
		new_parent.rh = child.rh;
		return new_parent;
	}

	template <typename U>
	T operator()(U const& x)
	{
		return rotate(std::is_same<T, U>(), x);
	}
};

struct precedence_visitor : public boost::static_visitor<ast::AstQueryType>
{
	ast::AstQueryType operator()(ast::Atom const& x)
	{
		return x;
	}

	ast::AstQueryType operator()(ast::Not const& x)
	{
		precedence_visitor precedence;
		ast::Not ret;
		ret.negated = boost::apply_visitor(precedence, x.negated);
		return ret;
	}

	ast::AstQueryType operator()(ast::Nest const& x)
	{
		precedence_visitor precedence;
		ast::Nest ret;
		ret.arg = boost::apply_visitor(precedence, x.arg);
		return ret;
	}

	template <typename T>
	ast::AstQueryType operator()(T const& x)
	{
		rotate_visitor<T> rotate(x);
		T ret = boost::apply_visitor(rotate, x.rh);

		precedence_visitor precedence;
		ret.rh = boost::apply_visitor(precedence, ret.rh);
		return ret;
	}
};

struct to_query_visitor : public boost::static_visitor<std::shared_ptr<Query>>
{
	template <typename T>
	std::shared_ptr<Query> operator()(T x)
	{
		return std::make_shared<typename T::QueryType>(x);
	}

	std::shared_ptr<Query> operator()(ast::Nest nest)
	{
		to_query_visitor v;
		return boost::apply_visitor(v, nest.arg);
	}
};

}

struct Query
{
	ast::to_query_visitor to_query;

	virtual ~Query(){}

	enum class field
	{
		artist,
		title,
		album
	};

	enum class modifier
	{
		contains,
		is
	};

	Query::field to_field(std::string const& x);
	Query::modifier to_modifier(std::string const& x);

	virtual std::vector<std::pair<size_t, LibraryEntry>> handle_search(std::multimap<std::hash<LibraryEntry>::result_type, EntryLocation> const& library) = 0;
};

struct Atom : public Query
{
	Query::field field;
	Query::modifier modifier;
	std::string query_string;

	Atom(ast::Atom const& ast);
	Atom(Query::field field, Query::modifier modifier, std::string query_string);

	std::string const& get_field_string(Query::field const& f, LibraryEntry const& entry);
	boost::regex const get_regex(Query::modifier const& m);

	std::vector<std::pair<size_t, LibraryEntry>> handle_search(std::multimap<std::hash<LibraryEntry>::result_type, EntryLocation> const& library) final;
};

struct And : public Query
{
	std::shared_ptr<Query> lh;
	std::shared_ptr<Query> rh;

	And(ast::And const& ast);
	std::vector<std::pair<size_t, LibraryEntry>> handle_search(std::multimap<std::hash<LibraryEntry>::result_type, EntryLocation> const& library) final;
};

struct Or : public Query
{
	std::shared_ptr<Query> lh;
	std::shared_ptr<Query> rh;

	Or(ast::Or const& ast);
	std::vector<std::pair<size_t, LibraryEntry>> handle_search(std::multimap<std::hash<LibraryEntry>::result_type, EntryLocation> const& library) final;
};

struct Not : public Query
{
	struct sequence_generator {
		int current;

		sequence_generator()
		: current(0)
		{}

		int operator()() {return current++;}
	};

	std::shared_ptr<Query> negated;

	Not(ast::Not const& ast);
	std::vector<std::pair<size_t, LibraryEntry>> handle_search(std::multimap<std::hash<LibraryEntry>::result_type, EntryLocation> const& library) final;
};

}
