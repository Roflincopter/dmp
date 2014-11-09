#pragma once

#include "radio_state.hpp"
#include "radio_list_ui_delegate.hpp"
#include "delegator.hpp"

#include <vector>
#include <string>
#include <map>

class RadioListModel : public Delegator<RadioListUiDelegate>
{
	std::vector<std::string> radio_names;
	std::string tuned_in_radio;
	
	std::map<std::string, RadioState> radio_states;

public:
	RadioListModel();

	int row_count() const;
	std::string get_data(int row) const;

	void set_radio_names(std::vector<std::string> new_radio_names);
	void add_radio(std::string radio_name);
	void remove_radio(std::string radio_name);
	std::string get_tuned_in_radio() const;
	void set_tuned_in_radio(std::string radio_name);
	
	void set_radio_states(std::map<std::string, RadioState> states);
	std::map<std::string, RadioState> get_radio_states() const;
};
