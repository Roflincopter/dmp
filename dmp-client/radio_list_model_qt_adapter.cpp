#include "radio_list_model_qt_adapter.hpp"

RadioListModelQtAdapter::RadioListModelQtAdapter()
{
}

int RadioListModelQtAdapter::rowCount(const QModelIndex&) const
{
	return row_count();
}

QVariant RadioListModelQtAdapter::data(const QModelIndex& index, int role) const
{
	if(role != Qt::DisplayRole) {
		return QVariant();
	}

	if(!index.isValid() && index.row() >= 0 && index.row() < rowCount(index)) {
		return QVariant();
	}

	return QVariant(QString::fromStdString(get_data(index.row())));
}

void RadioListModelQtAdapter::set_radio_names(std::vector<std::string> new_radio_names)
{
	beginResetModel();
	RadioListModel::set_radio_names(new_radio_names);
	endResetModel();
}
