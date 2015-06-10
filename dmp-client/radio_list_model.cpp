#include "radio_list_model.hpp"

#include "radio_list_ui_delegate.hpp"

#include "container_manipulations.hpp"

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
	call_on_delegates<RadioListUiDelegate>(&RadioListUiDelegate::set_radios_start);
	radio_names.push_back(radio_name);
	call_on_delegates<RadioListUiDelegate>(&RadioListUiDelegate::set_radios_end);
}

void RadioListModel::remove_radio(std::string radio_name)
{
	call_on_delegates<RadioListUiDelegate>(&RadioListUiDelegate::remove_radio_start);
	remove_element(radio_names, radio_name);
	call_on_delegates<RadioListUiDelegate>(&RadioListUiDelegate::remove_radio_end);
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
	call_on_delegates<RadioListUiDelegate>(&RadioListUiDelegate::set_radio_states);
}

std::map<std::string, RadioState> RadioListModel::get_radio_states() const
{
	return radio_states;
}
