#include "search_result_model_qt_adapter.hpp"
#include "boost_any_to_qvariant.hpp"

#include <boost/any.hpp>

#include <boost/fusion/iterator/advance.hpp>
#include <boost/fusion/include/advance.hpp>

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
	} catch (std::out_of_range &e) {
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
	} catch(std::out_of_range &e) {
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

void SearchResultModelQtAdapter::search_results_start(message::SearchResponse response)
{
	beginInsertRows(QModelIndex(), model->row_count(), response.results.size() - 1);
}

void SearchResultModelQtAdapter::search_results_end()
{
	endInsertRows();
}

auto SearchResultModelQtAdapter::get_row_info(int index) -> decltype(model->get_row_info(index))
{
	return model->get_row_info(index);
}
