#pragma once

#include "library-entry.hpp"

#include <memory>
#include <vector>

namespace dmp_library {

struct query 
{
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
	
	virtual std::vector<size_t> handle_search(std::vector<library_entry> const& library) = 0;
};	

struct atom : public query
{
	query::field field;
	query::modifier modifier;
	std::string query_string;
	
	atom(query::field field, query::modifier modifier, std::string query_string);
	std::vector<size_t> handle_search(std::vector<library_entry> const& library) final;
};

struct _and : public query
{
	std::shared_ptr<query> lh;
	std::shared_ptr<query> rh;
	
	_and(std::shared_ptr<query> lh, std::shared_ptr<query> rh);
	std::vector<size_t> handle_search(std::vector<library_entry> const& library) final;
};

struct _or : public query
{
	std::shared_ptr<query> lh;
	std::shared_ptr<query> rh;
	
	_or(std::shared_ptr<query> lh, std::shared_ptr<query> rh);
	std::vector<size_t> handle_search(std::vector<library_entry> const& library) final;
};

struct _not : public query
{
	std::shared_ptr<query> arg;
	
	_not(std::shared_ptr<query> arg);
	std::vector<size_t> handle_search(std::vector<library_entry> const& library) final;
};

}