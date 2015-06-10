#pragma once

#include "delegator.hpp"

#include <string>
#include <stddef.h>

struct SearchBarUiDelegate;

class SearchBarModel
: public Delegator<SearchBarUiDelegate>
{
	std::string query;
	std::string expected;
	size_t pivot;

public:
	SearchBarModel();

	void set_data(std::string new_expected, size_t new_pivot);
	void set_query(std::string new_query);

	std::string get_expected() const;
	size_t get_pivot() const;
	size_t get_query_length() const;
};
