#define once

#include <string>
#include <cstdint>

class SearchBarModel
{
	std::string expected;
	size_t pivot;

public:
	SearchBarModel();

	virtual void set_data(std::string new_expected, size_t new_pivot);

	std::string& get_expected();
	size_t& get_pivot();
};
