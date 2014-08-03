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
	virtual void timerEvent(QTimerEvent*);
	void setEnabled(bool enabled);

	// Facilitate error reporting between threads
	// (these methods are thread-safe)
	std::mutex error_indices_mutex;
	std::map<int, std::string> error_indices;
	void setErrorIndex(int index, std::string error);
	std::string getErrorIndex(int index);
	void clearErrorIndex(int index);

public:
	explicit DmpClientGui(QWidget *parent = 0);
	~DmpClientGui() = default;

	void connect_client(std::string name, std::string host, uint16_t port);

	void set_client(std::shared_ptr<DmpClientInterface> new_client);

	void closeEvent(QCloseEvent*) override final;
	
	void set_play_paused_state(bool state) override final;
private slots:
	void test1();
	void test2();

public slots:
	void dmpConnect();
	void StopPressed();
	void PlayPauseToggled(bool state);
	void NextPressed();
	void MuteToggled(bool state);

private:
	Ui::DmpClientGui ui;
};
