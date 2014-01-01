#pragma once

#include "playlists_model.hpp"

#include <QAbstractTableModel>

class PlaylistsModelQtAdapter : public PlaylistsModel, public QAbstractTableModel
{
public:
	PlaylistsModelQtAdapter();

	int rowCount(const QModelIndex & = QModelIndex()) const final;
	int columnCount(const QModelIndex &) const final;
	QVariant data(const QModelIndex &index, int role) const final;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const final;

	virtual void update_playlist(std::string radio_name, std::vector<std::tuple<std::string, std::string, dmp_library::LibraryEntry>> playlist) final;
	virtual void set_current_radio(std::string radio_name) final;
};
