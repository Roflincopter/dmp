#pragma once

#include "search_result_model_qt_adapter.hpp"
#include "dmp_client_interface.hpp"
#include "dmp_client_ui_controller_interface.hpp"
#include "dmp_client_ui_delegate.hpp"

#include <QTableView>

class DmpClientGuiSearchResults : public QTableView, public DmpClientUiControllerInterface, public DmpClientUiDelegate
{
	Q_OBJECT

	std::shared_ptr<DmpClientInterface> client;
	SearchResultModelQtAdapter model;

	std::string current_active_radio;
	std::string current_query;

public:
	explicit DmpClientGuiSearchResults(QWidget *parent = 0);

	virtual void set_client(std::shared_ptr<DmpClientInterface> new_client) final;

	virtual void search_results(message::SearchResponse search_response) final;
	virtual void new_search(std::string query) final;
signals:

public slots:
	void queueRequest(QModelIndex index);
	void currentActiveRadio(std::string radio_name);
};
