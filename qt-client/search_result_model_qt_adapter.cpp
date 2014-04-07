#include "search_result_model_qt_adapter.hpp"
#include "boost_any_to_qvariant.hpp"

#include <boost/any.hpp>

#include <boost/fusion/iterator/advance.hpp>
#include <boost/fusion/include/advance.hpp>

SearchResultModelQtAdapter::SearchResultModelQtAdapter()
{
}

void SearchResultModelQtAdapter::add_search_response(message::SearchResponse search_response)
{
	if(search_response.results.size() == 0) {
		return;
	}
	beginInsertRows(QModelIndex(), rowCount(), rowCount() + search_response.results.size() - 1);
	model->add_search_response(search_response);
	endInsertRows();
}

void SearchResultModelQtAdapter::clear()
{
	beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
	model->clear();
	endRemoveRows();
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
	} catch (std::out_of_range e) {
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
	} catch(std::out_of_range e) {
		return QVariant();
	}
}

auto SearchResultModelQtAdapter::get_row_info(int index) -> decltype(model->get_row_info(index))
{
	return model->get_row_info(index);
}
