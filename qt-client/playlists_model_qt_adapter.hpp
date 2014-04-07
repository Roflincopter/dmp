#pragma once

#include "qt_adapter.hpp"
#include "playlists_model.hpp"

#include <QAbstractTableModel>

class PlaylistsModelQtAdapter : public QtAdapter<PlaylistsModel>, public QAbstractTableModel
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

	void update(std::string radio_name, Playlist playlist);
	void append(std::string radio_name, Playlist playlist);
	void reset(std::string radio_name);

	auto create_radio(std::string radio_name) -> decltype(model->create_radio(radio_name));
	void set_current_radio(std::string radio_name);
};
