#include "dmp_client_gui_search_results.hpp"

#include "search_result_model_qt_adapter.hpp"
#include "search_result_sort_proxy_model.hpp"

#include "dmp_client_interface.hpp"
#include "search_result_model.hpp"

#include <QHeaderView>
#include <QAbstractItemModel>
#include <QItemSelectionModel>

#include <tuple>

class QWidget;

DmpClientGuiSearchResults::DmpClientGuiSearchResults(QWidget *parent)
: QTableView(parent)
, client(nullptr)
, model(std::make_shared<SearchResultModelQtAdapter>())
, proxy_model(std::make_shared<SearchResultSortProxyModel>(model))
, delegate()
{
	setModel(proxy_model.get());
	setItemDelegate(&delegate);
	hideColumn(1);
	hideColumn(3);
	hideColumn(5);
	horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void DmpClientGuiSearchResults::set_client(std::shared_ptr<DmpClientInterface> new_client)
{
	client = new_client;
	model->set_model(client->get_search_result_model());
	client->get_search_result_model()->add_delegate<SearchResultUiDelegate>(shared_from_this());
	client->get_search_result_model()->add_delegate<SearchResultUiDelegate>(model);
}

void DmpClientGuiSearchResults::queueRequest(QModelIndex index)
{
	index = proxy_model->mapToSource(index);
	auto x = model->get_row_info(index.row());
	if(!current_active_radio.empty()) {
		client->queue(current_active_radio, std::get<0>(x), std::get<1>(x));
	}
}

void DmpClientGuiSearchResults::queueSelection()
{
	auto selection = selectionModel()->selectedRows();
	for(auto row_index : selection) {
		queueRequest(row_index);
	}
}

void DmpClientGuiSearchResults::currentActiveRadio(std::string radio_name)
{
	current_active_radio = radio_name;
}
