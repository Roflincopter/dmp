#pragma once

#include "gui_item_delegate.hpp"

#include "playlist_ui_delegate.hpp"
#include "dmp_client_ui_controller_interface.hpp"

#include <qobjectdefs.h>
#include <QTableView>

#include <memory>
#include <string>

class PlaylistsModelQtAdapter;
class QWidget;
struct DmpClientInterface;

class DmpClientGuiPlaylists 
: public QTableView
, public DmpClientUiControllerInterface
, public PlaylistUiDelegate
, public std::enable_shared_from_this<PlaylistUiDelegate>
{
	Q_OBJECT

	std::shared_ptr<DmpClientInterface> client;
	std::shared_ptr<PlaylistsModelQtAdapter> model;
	GuiItemDelegate delegate;
	
	void move_selected(bool up);

public:
	explicit DmpClientGuiPlaylists(QWidget *parent = 0);

	virtual void set_client(std::shared_ptr<DmpClientInterface> new_client) override final;

signals:

public slots:
	void currentRadioChanged(std::string name);
	void deleteSelected();
	void moveSelectedUp();
	void moveSelectedDown();
};
