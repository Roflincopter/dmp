#pragma once

#include "search_result_model.hpp"

#include <QAbstractTableModel>

class SearchResultModelQtAdapter : public SearchResultModel, public QAbstractTableModel
{
public:
	SearchResultModelQtAdapter();

	virtual void add_search_response(message::SearchResponse search_response) override final;
	virtual void clear() override final;

	int rowCount(const QModelIndex & = QModelIndex()) const override final;
	int columnCount(const QModelIndex &) const override final;
	QVariant data(const QModelIndex &index, int role) const override final;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override final;
};
