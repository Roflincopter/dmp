#include "radio_list_model_qt_adapter.hpp"

RadioListModelQtAdapter::RadioListModelQtAdapter()
{
}

void RadioListModelQtAdapter::set_model(std::shared_ptr<RadioListModel> new_model)
{
	model = new_model;
	model->add_delegate<RadioListUiDelegate>(shared_from_this());
}

int RadioListModelQtAdapter::rowCount(const QModelIndex&) const
{
	return model->row_count();
}

QVariant RadioListModelQtAdapter::data(const QModelIndex& index, int role) const
{
	if(role != Qt::DisplayRole) {
		return QVariant();
	}

	if(!index.isValid() && index.row() >= 0 && index.row() < rowCount(index)) {
		return QVariant();
	}

	return QVariant(QString::fromStdString(model->get_data(index.row())));
}

QModelIndex RadioListModelQtAdapter::get_model_index_for(int row)
{
	return createIndex(row, 0);
}

void RadioListModelQtAdapter::set_radios_start()
{
	beginResetModel();
}

void RadioListModelQtAdapter::set_radios_end()
{
	endResetModel();
}

void RadioListModelQtAdapter::add_radio_start()
{
	beginInsertRows(QModelIndex(), model->row_count(), model->row_count());
}

void RadioListModelQtAdapter::add_radio_end()
{
	endInsertRows();
}

void RadioListModelQtAdapter::remove_radio_start()
{
	beginResetModel();
}

void RadioListModelQtAdapter::remove_radio_end()
{
	endResetModel();
}
