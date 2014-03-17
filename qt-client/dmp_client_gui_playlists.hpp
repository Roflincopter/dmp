#pragma once

#include "playlists_model_qt_adapter.hpp"
#include "gui_item_delegate.hpp"

#include "dmp_client_ui_delegate.hpp"
#include "dmp_client_ui_controller_interface.hpp"

#include <QTableView>

class DmpClientGuiPlaylists : public QTableView, public DmpClientUiDelegate, public DmpClientUiControllerInterface
{
	Q_OBJECT

	std::shared_ptr<DmpClientInterface> client;
	PlaylistsModelQtAdapter model;
	GuiItemDelegate delegate;

public:
	explicit DmpClientGuiPlaylists(QWidget *parent = 0);

	virtual void set_client(std::shared_ptr<DmpClientInterface> new_client) override final;

	virtual void radios_update(message::Radios update) override final;
	virtual void playlist_updated(message::PlaylistUpdate update) override final;
	virtual void add_radio_succes(message::AddRadioResponse response) override final;

signals:

public slots:
	void currentRadioChanged(std::string name);
};