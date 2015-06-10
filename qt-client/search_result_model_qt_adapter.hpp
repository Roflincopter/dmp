#pragma once

#include "qt_adapter.hpp"
#include "search_result_model.hpp"
#include "search_result_ui_delegate.hpp"

#include <QAbstractItemModel>
#include <QVariant>

#include <memory>

namespace message { struct SearchResponse; }

class SearchResultModelQtAdapter : public QtAdapter<SearchResultModel>, public QAbstractTableModel, public SearchResultUiDelegate
{
public:
	SearchResultModelQtAdapter();

	void add_search_response(message::SearchResponse search_response);

	int rowCount(const QModelIndex & = QModelIndex()) const override final;
	int columnCount(const QModelIndex &) const override final;
	virtual QVariant data(const QModelIndex &index, int role) const override final;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override final;

	virtual void new_search_begin() override final;
	virtual void new_search_end() override final;
	
	virtual void search_results_start(int count) override final;
	virtual void search_results_end() override final;
	
	auto get_row_info(int index) -> decltype(model->get_row_info(index));
};
