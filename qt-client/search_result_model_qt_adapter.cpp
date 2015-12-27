#include "search_result_model_qt_adapter.hpp"

#include "dmp_qt_meta_types.hpp"
#include "boost_any_qvariant_convert.hpp"

#include <QString>

#include <stdexcept>

SearchResultModelQtAdapter::SearchResultModelQtAdapter()
{
}

int SearchResultModelQtAdapter::rowCount(const QModelIndex&) const
{
	return model->row_count();
}

int SearchResultModelQtAdapter::columnCount(const QModelIndex&) const
{
	return model->column_count();
}

QVariant SearchResultModelQtAdapter::data(const QModelIndex &index, int role) const
{
	if (role != Qt::DisplayRole) {
		return QVariant();
	}
	if(!index.isValid()) {
		return QVariant();
	}
	try {
		return QVariant(to_qvariant<SearchResultModel::ElementType>(model->get_cell(index.row(), index.column()), index.column()));
	} catch (std::out_of_range&) {
		return QVariant();
	}
}

QVariant SearchResultModelQtAdapter::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Vertical) {
		return QVariant();
	}

	if (role != Qt::DisplayRole) {
		return QVariant();
	}

	if(section < 0) {
		return QVariant();
	}

	try {
		return QVariant(QString::fromStdString(model->header_data(section)));
	} catch(std::out_of_range&) {
		return QVariant();
	}
}

void SearchResultModelQtAdapter::new_search_begin()
{
	beginResetModel();
}

void SearchResultModelQtAdapter::new_search_end()
{
	endResetModel();
}

void SearchResultModelQtAdapter::clear_start()
{
	beginResetModel();
}

void SearchResultModelQtAdapter::clear_end()
{
	endResetModel();
}

void SearchResultModelQtAdapter::remove_entries_start(int start, int count)
{
	std::cout << "Start: " << start << "count: " << count << std::endl;
	beginRemoveRows(QModelIndex(), start, start + count);
}

void SearchResultModelQtAdapter::remove_entries_end()
{
	endRemoveRows();
}

void SearchResultModelQtAdapter::search_results_start(int count)
{
	beginInsertRows(QModelIndex(), model->row_count(), model->row_count() + count - 1);
}

void SearchResultModelQtAdapter::search_results_end()
{
	endInsertRows();
}

auto SearchResultModelQtAdapter::get_row_info(int index) -> decltype(model->get_row_info(index))
{
	return model->get_row_info(index);
}
