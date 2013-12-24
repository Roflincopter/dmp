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
