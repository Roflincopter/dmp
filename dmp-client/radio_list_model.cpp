#include "radio_list_model.hpp"

RadioListModel::RadioListModel()
{
}

int RadioListModel::row_count() const
{
	return radio_names.size();
}

std::string RadioListModel::get_data(int row) const
{
	return radio_names[row];
}

void RadioListModel::set_radio_names(std::vector<std::string> new_radio_names)
{
	radio_names = new_radio_names;
}

void RadioListModel::add_radio(std::string radio_name)
{
	radio_names.push_back(radio_name);
}

std::string RadioListModel::get_tuned_in_radio() const
{
	return tuned_in_radio;
}

void RadioListModel::set_tuned_in_radio(std::string radio_name)
{
	tuned_in_radio = radio_name;
}

void RadioListModel::set_radio_states(std::map<std::string, RadioState> states)
{
	radio_states = states;
}

std::map<std::string, RadioState> RadioListModel::get_radio_states() const
{
	return radio_states;
}
