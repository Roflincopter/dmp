#pragma once

#include <vector>
#include <string>

class RadioListModel
{
	std::vector<std::string> radio_names;

public:
	RadioListModel();

	int row_count() const;
	std::string get_data(int row) const;

	virtual void set_radio_names(std::vector<std::string> new_radio_names);
	virtual void add_radio(std::string radio_name);
};
