#pragma once

#include "library-entry.hpp"

#include <boost/variant.hpp>
#include <boost/variant/static_visitor.hpp>

#include <memory>
#include <vector>
#include <iostream>

namespace dmp_library {

struct ast_and;
struct ast_or;
struct ast_not;
struct ast_atom;
struct ast_nest;

struct query;
struct atom;
struct _not;
struct _and;
struct _or;

typedef boost::variant<ast_atom, boost::recursive_wrapper<ast_or>, boost::recursive_wrapper<ast_and>, boost::recursive_wrapper<ast_not>, boost::recursive_wrapper<ast_nest>> ast_query_type;
	
struct print_visitor : public boost::static_visitor<std::ostream&>
{
	std::ostream& os;

	print_visitor(std::ostream& os)
	: os(os)
	{}

	template <typename T>
	std::ostream& operator()(T x)
	{
		return os << x;
	}
};

struct ast_atom {
	std::string field;
	std::string modifier;
	std::string input;
	
	typedef atom query_type;
	
	friend std::ostream& operator<<(std::ostream& os, ast_atom const& atom)
	{
		std::cout << atom.field << " " << atom.modifier << " " << atom.input;
	}
};

struct ast_not {
	ast_query_type negated;
	
	typedef _not query_type;
	
	friend std::ostream& operator<<(std::ostream& os, ast_not const& _not)
	{
		os << " not: ";
		print_visitor p(os);
		boost::apply_visitor(p, _not.negated);
		return os;
	}
};

struct ast_or {
	ast_query_type lh;
	ast_query_type rh;
	
	typedef _or query_type;
	
	friend std::ostream& operator<<(std::ostream& os, ast_or const& _or)
	{
		print_visitor p(os);
		boost::apply_visitor(p, _or.lh);
		os << " :or: ";
		boost::apply_visitor(p, _or.rh);
		return os;
	}
};

struct ast_and {
	ast_query_type lh;
	ast_query_type rh;
	
	typedef _and query_type;
	
	friend std::ostream& operator<<(std::ostream& os, ast_and const& _and)
	{
		print_visitor p(os);
		boost::apply_visitor(p, _and.lh);
		os << " :and: ";
		boost::apply_visitor(p, _and.rh);
		return os;
	}
};

struct ast_nest {
	ast_query_type arg;
	
	friend std::ostream& operator<<(std::ostream& os, ast_nest const& nest)
	{
		os << " (";
		print_visitor p(os);
		boost::apply_visitor(p, nest.arg);
		os << ") ";
		return os;
	}
};

struct to_query_visitor : public boost::static_visitor<std::shared_ptr<query>>
{
	template <typename T>
	std::shared_ptr<query> operator()(T x)
	{
		return std::make_shared<typename T::query_type>(x);
	}
	
	std::shared_ptr<query> operator()(ast_nest nest)
	{
		to_query_visitor v;
		boost::apply_visitor(v, nest.arg);
	}
};

struct query 
{
	to_query_visitor to_query;
	
	virtual ~query(){}
	
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
	
	query::field to_field(std::string const& x);
	query::modifier to_modifier(std::string const& x);
	
	virtual std::vector<size_t> handle_search(std::vector<library_entry> const& library) = 0;
};	

struct atom : public query
{
	query::field field;
	query::modifier modifier;
	std::string query_string;
	
	atom(ast_atom const& ast)
	: field(to_field(ast.field))
	, modifier(to_modifier(ast.modifier))
	, query_string(ast.input)
	{}
	
	atom(query::field field, query::modifier modifier, std::string query_string);
	std::vector<size_t> handle_search(std::vector<library_entry> const& library) final;
};

struct _and : public query
{
	std::shared_ptr<query> lh;
	std::shared_ptr<query> rh;
	
	_and(ast_and const& ast);
	_and(std::shared_ptr<query> lh, std::shared_ptr<query> rh);
	std::vector<size_t> handle_search(std::vector<library_entry> const& library) final;
};

struct _or : public query
{
	std::shared_ptr<query> lh;
	std::shared_ptr<query> rh;
	
	_or(ast_or const& ast);	
	_or(std::shared_ptr<query> lh, std::shared_ptr<query> rh);
	std::vector<size_t> handle_search(std::vector<library_entry> const& library) final;
};

struct _not : public query
{
	std::shared_ptr<query> negated;
	
	_not(ast_not const& ast);
	_not(std::shared_ptr<query> arg);
	std::vector<size_t> handle_search(std::vector<library_entry> const& library) final;
};

}