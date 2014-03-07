#pragma once

#include "playlists_model.hpp"

#include <QAbstractTableModel>

class PlaylistsModelQtAdapter : public PlaylistsModel, public QAbstractTableModel
{
public:
	PlaylistsModelQtAdapter();

	int rowCount(const QModelIndex & = QModelIndex()) const override final;
	int columnCount(const QModelIndex &) const override final;
	QVariant data(const QModelIndex &index, int role) const override final;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override final;

	virtual void update_playlist(std::string radio_name, Playlist playlist) override final;
	virtual void append(std::string radio_name, Playlist playlist) override final;

	virtual void set_current_radio(std::string radio_name) override final;
};
