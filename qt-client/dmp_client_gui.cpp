#include "dmp_client_gui.hpp"

#include "debug_macros.hpp"
#include "connect.hpp"
#include "dmp_client_connect_dialog.hpp"
#include "dmp_client_login_dialog.hpp"
#include "dmp_client_error_dialog.hpp"
#include "dmp_client_register_dialog.hpp"
#include "call_event.hpp"
#include "dmp_client_ui_delegate.hpp"

#include <QFileDialog>
#include <QMessageBox>
#include <QKeySequence>

#include <boost/asio/ip/host_name.hpp>

#include <iostream>
#include <string>

DmpClientGui::DmpClientGui(QWidget *parent)

: QMainWindow(parent)
, search_result_model()
, client(nullptr)
, client_thread()
, shared_main_window(nullptr)
, shared_menu_bar(nullptr)
, shared_search_bar(nullptr)
, shared_search_results(nullptr)
, shared_radio_list(nullptr)
, shared_playlists(nullptr)
, ui()
{
	ui.setupUi(this);

	shared_main_window = std::shared_ptr<DmpClientGui>(this, [](void*){});
	shared_menu_bar = std::shared_ptr<DmpClientGuiMenuBar>(ui.menu_bar, [](void*){});
	shared_search_bar = std::shared_ptr<DmpClientGuiSearchBar>(ui.search_bar, [](void*){});
	shared_search_results = std::shared_ptr<DmpClientGuiSearchResults>(ui.search_results, [](void*){});
	shared_radio_list = std::shared_ptr<DmpClientGuiRadioList>(ui.radio_list, [](void*){});
	shared_playlists = std::shared_ptr<DmpClientGuiPlaylists>(ui.playlists, [](void*){});

	setEnabled(false);
}

void DmpClientGui::update_ui_client_interface()
{
	client->add_delegate(shared_main_window);
	
	shared_menu_bar->set_client(client);
	shared_search_bar->set_client(client);
	shared_search_results->set_client(client);
	shared_radio_list->set_client(client);
	shared_playlists->set_client(client);
}

void DmpClientGui::set_client(std::shared_ptr<DmpClientInterface> new_client)
{
	if(client) {
		client->destroy();
	}
	
	if(client_thread.joinable()) {
		client_thread.join();
	}

	client = new_client;

	auto client_runner = [this]
	{
		try {
			client->run();
		} catch (std::exception &e) {
			std::string message = e.what();
			QApplication::postEvent(this, new CallEvent([this, message]{error(message);}));
		}
		client->destroy();
		client.reset();
		setEnabled(false);
	};

	client_thread = std::thread(client_runner);

	update_ui_client_interface();
}

bool DmpClientGui::connect_client(std::string host, uint16_t port)
{
	try {
		auto client_sp = std::make_shared<DmpClient>(host, port);
		set_client(client_sp);
	} catch(std::exception &e) {
		DmpClientErrorDialog dialog("Failed to connect to " + host + ":" + std::to_string(port) + ": " + e.what());
		dialog.exec();
		setEnabled(false);
		return false;
	}
	return true;
}

void DmpClientGui::login_client(std::string username, std::string password)
{
	client->send_login(username, password);
}

void DmpClientGui::setEnabled(bool enabled) {
	ui.search_bar->setEnabled(enabled);
	ui.addRadioButton->setEnabled(enabled);
	ui.deleteRadioButton->setEnabled(enabled);
	ui.tuneInRadioButton->setEnabled(enabled);

	ui.stopButton->setEnabled(enabled);
	ui.playButton->setEnabled(enabled);
	ui.nextButton->setEnabled(enabled);
	ui.muteButton->setEnabled(enabled);
	ui.volumeSlider->setEnabled(enabled);
}

void DmpClientGui::register_user()
{
	DmpClientRegisterDialog register_acc;
	int result = register_acc.exec();
	
	if(result == QDialog::Rejected) {
		login_user();
	}
	
	if(result == QDialog::Accepted) {
		client->register_user(register_acc.get_username(), register_acc.get_password());
	}
}

void DmpClientGui::login_user()
{
	DmpClientLoginDialog login;
	int result = login.exec();
	
	if(result == QDialog::Rejected) {
		client->send_bye();
	}
	
	if(result == DmpClientLoginDialog::Register) {
		register_user();
	}
	
	if(result == QDialog::Accepted) {
		login_client(login.get_username(), login.get_password());
	}
}

void DmpClientGui::dmpConnect()
{
	DmpClientConnectDialog connect;
	int result = connect.exec();

	if(result != QDialog::Accepted) {
		return;
	}

	if(connect_client(connect.get_host(), connect.get_port())) {
		login_user();
	} else {
		QApplication::postEvent(this, new CallEvent([this]{dmpConnect();}));
	}
}

void DmpClientGui::StopPressed()
{
	ui.playButton->setChecked(false);
	client->stop_radio();
}

void DmpClientGui::PlayPauseToggled(bool state)
{
	if(state) {
		client->play_radio();
	} else {
		client->pause_radio();
	}
}

void DmpClientGui::set_play_paused_state(bool state)
{
	ui.playButton->setChecked(state);
}

void DmpClientGui::login_succeeded()
{
	setEnabled(true);
}

void DmpClientGui::login_failed(std::string reason)
{
	QApplication::postEvent(this, new CallEvent([this]{login_user();}));
	QApplication::postEvent(this, new CallEvent([this, reason]{error(reason);}));
}

void DmpClientGui::register_succeeded()
{
	QApplication::postEvent(this, new CallEvent([this]{login_user();}));
}

void DmpClientGui::register_failed(std::string reason)
{
	QApplication::postEvent(this, new CallEvent([this]{register_user();}));
	QApplication::postEvent(this, new CallEvent([this, reason]{error(reason);}));
}

void DmpClientGui::volume_changed(int volume)
{
	ui.volumeSlider->setValue(volume);
}

void DmpClientGui::NextPressed()
{
	client->next_radio();
}

void DmpClientGui::MuteToggled(bool state)
{
	client->mute_radio(state);
}

void DmpClientGui::VolumeChanged(int volume)
{
	client->change_volume(volume);
}

void DmpClientGui::closeEvent(QCloseEvent*)
{
	if(!client) {
		if(client_thread.joinable()) {
			client_thread.join();
		}
		return;
	}

	client->stop();
	client_thread.join();
}

void DmpClientGui::error(std::string error) {
	DmpClientErrorDialog(error, this).exec();
}

bool DmpClientGui::event(QEvent *event) {
	if(CallEvent* call = dynamic_cast<CallEvent*>(event)) {
		(*call)();
		call->accept();
		return true;
	} else {
		return QMainWindow::event(event);
	}
}
