#pragma once

#include "search_result_model.hpp"
#include "dmp_client.hpp"
#include "dmp_client_interface.hpp"
#include "ui_dmp_client_gui.hpp"
#include "dmp_client_ui_delegate.hpp"

#include <QMainWindow>
#include <QShortcut>

#include <thread>
#include <memory>
#include <map>
#include <mutex>

class DmpClientGui : public QMainWindow, public DmpClientUiDelegate
{
	Q_OBJECT

	SearchResultModel search_result_model;
	std::shared_ptr<DmpClientInterface> client;
	std::thread client_thread;

	QShortcut test1_key;
	QShortcut test2_key;
	QShortcut test3_key;

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
	std::shared_ptr<DmpClientGuiPlaylists> shared_playlists;

	void update_ui_client_interface();
	void setEnabled(bool enabled);

public:
	explicit DmpClientGui(QWidget *parent = 0);
	~DmpClientGui() = default;

	void error(std::string error);

	// reimplementation of event to route custom events to the gui main thread.
	bool event(QEvent* event) override final;

	bool connect_client(std::string host, uint16_t port);
	void login_client(std::string username, std::string password);

	void set_client(std::shared_ptr<DmpClientInterface> new_client);

	void register_user();
	void login_user();

	void closeEvent(QCloseEvent*) override final;

	void set_play_paused_state(bool state) override final;
	
	void login_succeeded() override final;
	void login_failed(std::string reason) override final;
	
	void register_succeeded() override final;
	void register_failed(std::string reason) override final;
	
private slots:
	void test1();
	void test2();
	void test3();

public slots:
	void dmpConnect();
	void StopPressed();
	void PlayPauseToggled(bool state);
	void NextPressed();
	void MuteToggled(bool state);

private:
	Ui::DmpClientGui ui;
};
