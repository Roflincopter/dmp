#pragma once

#include "qt_adapter.hpp"
#include "search_result_model.hpp"

#include <QAbstractTableModel>

class SearchResultModelQtAdapter : public QtAdapter<SearchResultModel>, public QAbstractTableModel
{
public:
	SearchResultModelQtAdapter();

	void add_search_response(message::SearchResponse search_response);
	void clear();

	int rowCount(const QModelIndex & = QModelIndex()) const override final;
	int columnCount(const QModelIndex &) const override final;
	QVariant data(const QModelIndex &index, int role) const override final;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override final;
	
	auto get_row_info(int index) -> decltype(model->get_row_info(index));
};
