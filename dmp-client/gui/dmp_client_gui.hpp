#pragma once

#include "search_result_model.hpp"
#include "dmp_client.hpp"
#include "dmp_client_interface.hpp"
#include "ui_dmp_client_gui.hpp"
#include "dmp_client_ui_delegate.hpp"

#include <QMainWindow>
#include <QStandardItemModel>

#include <thread>
#include <memory>

class DmpClientGui : public QMainWindow, public DmpClientUiDelegate
{
	Q_OBJECT

	SearchResultModel search_result_model;
	std::shared_ptr<DmpClientInterface> client;
	std::thread client_thread;

	/* these variables hold non-owning shared pointers to the raw pointers
	*  provided by the QtDesigner generated code. Be very cautious what you
	*  do with these. They should only be used as weak_pointers and no other.
	*  shared pointers should leave this class instance.
	*/
	std::shared_ptr<DmpClientGui> shared_main_window;
	std::shared_ptr<DmpClientGuiMenuBar> shared_menu_bar;
	std::shared_ptr<DmpClientGuiSearchBar> shared_search_bar;
	std::shared_ptr<DmpClientGuiSearchResults> shared_search_results;
	std::shared_ptr<DmpClientGuiRadioList> shared_radio_list;

	void update_ui_client_interface();

public:
	explicit DmpClientGui(QWidget *parent = 0);
	~DmpClientGui() = default;

	void set_client(std::shared_ptr<DmpClientInterface> new_client);

	void bye_ack_received() final;

	void closeEvent(QCloseEvent*) final;

public slots:
	void dmpConnect();

private:
	std::unique_ptr<Ui::DmpClientGui> ui;
	std::thread client_synchronisation_thread;
};
