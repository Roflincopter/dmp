#pragma once

#include "qt_adapter.hpp"
#include "playlists_model.hpp"
#include "dmp_client_ui_delegate.hpp"

#include <QAbstractTableModel>

class PlaylistsModelQtAdapter : public DmpClientUiDelegate, public QtAdapter<PlaylistsModel>, public QAbstractTableModel
{
private:
	bool should_update_view(std::string radio_name);
public:
	PlaylistsModelQtAdapter();
	
	void set_model(std::shared_ptr<PlaylistsModel> model);

	int rowCount(const QModelIndex & = QModelIndex()) const override final;
	int columnCount(const QModelIndex &) const override final;
	QVariant data(const QModelIndex &index, int role) const override final;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override final;

	virtual void playlist_update_start(message::PlaylistUpdate update) override final;
	virtual void playlist_update_end(message::PlaylistUpdate update) override final;
	
	virtual void set_radios_start() override final;
	virtual void set_radios_end() override final;
};
