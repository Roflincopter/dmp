#pragma once

#include <string>
#include <cstdint>

class SearchBarModel
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
