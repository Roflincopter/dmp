#pragma once

#include "qt_adapter.hpp"
#include "search_result_model.hpp"
#include "dmp_client_ui_delegate.hpp"
#include "search_result_ui_delegate.hpp"

#include <QAbstractTableModel>

class SearchResultModelQtAdapter : public QtAdapter<SearchResultModel>, public QAbstractTableModel, public SearchResultUiDelegate
{
public:
	SearchResultModelQtAdapter();

	void add_search_response(message::SearchResponse search_response);

	int rowCount(const QModelIndex & = QModelIndex()) const override final;
	int columnCount(const QModelIndex &) const override final;
	QVariant data(const QModelIndex &index, int role) const override final;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override final;

	virtual void new_search_begin() override final;
	virtual void new_search_end() override final;
	
	virtual void search_results_start(message::SearchResponse response) override final;
	virtual void search_results_end() override final;
	
	auto get_row_info(int index) -> decltype(model->get_row_info(index));
};
