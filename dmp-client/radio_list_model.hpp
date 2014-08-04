#pragma once

#include <vector>
#include <string>

class RadioListModel
{
	std::vector<std::string> radio_names;
	std::string tuned_in_radio;
public:
	RadioListModel();

	int row_count() const;
	std::string get_data(int row) const;

	void set_radio_names(std::vector<std::string> new_radio_names);
	void add_radio(std::string radio_name);
	std::string get_tuned_in_radio() const;
	void set_tuned_in_radio(std::string radio_name);
};
