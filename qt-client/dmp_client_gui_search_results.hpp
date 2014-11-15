#pragma once

#include "search_result_model_qt_adapter.hpp"
#include "search_result_sort_proxy_model.hpp"
#include "dmp_client_interface.hpp"
#include "dmp_client_ui_controller_interface.hpp"
#include "search_result_ui_delegate.hpp"
#include "gui_item_delegate.hpp"

#include <QTableView>

class DmpClientGuiSearchResults
: public QTableView
, public DmpClientUiControllerInterface
, public SearchResultUiDelegate
, public std::enable_shared_from_this<SearchResultUiDelegate>
{
	Q_OBJECT

	std::shared_ptr<DmpClientInterface> client;
	std::shared_ptr<SearchResultModelQtAdapter> model;
	std::shared_ptr<SearchResultSortProxyModel> proxy_model;
	GuiItemDelegate delegate;

	std::string current_active_radio;
	std::string current_query;

public:
	explicit DmpClientGuiSearchResults(QWidget *parent = 0);

	virtual void set_client(std::shared_ptr<DmpClientInterface> new_client) override final;

signals:

public slots:
	void queueRequest(QModelIndex index);
	void currentActiveRadio(std::string radio_name);
};
