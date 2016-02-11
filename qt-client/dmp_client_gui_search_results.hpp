#pragma once

#include "gui_item_delegate.hpp"

#include "dmp_client_ui_controller_interface.hpp"
#include "search_result_ui_delegate.hpp"

#include <QTableView>

#include <memory>
#include <string>

class QModelIndex;
class QWidget;
class SearchResultModelQtAdapter;
class SearchResultSortProxyModel;
struct DmpClientInterface;

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
	void queueSelection();
	void currentActiveRadio(std::string radio_name);
};
