#include "playlists_model_qt_adapter.hpp"

PlaylistsModelQtAdapter::PlaylistsModelQtAdapter()
{
}

int PlaylistsModelQtAdapter::rowCount(QModelIndex const&) const
{
	return row_count();
}

int PlaylistsModelQtAdapter::columnCount(QModelIndex const&) const
{
	return column_count();
}

QVariant PlaylistsModelQtAdapter::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation == Qt::Vertical) {
		return QVariant();
	}

	if(role != Qt::DisplayRole) {
		return QVariant();
	}

	return QVariant(QString::fromStdString(header_data(section)));
}

QVariant PlaylistsModelQtAdapter::data(QModelIndex const& index, int role) const
{
	if(role != Qt::DisplayRole) {
		return QVariant();
	}

	return QVariant(QString::fromStdString(get_cell(index.row(), index.column())));
}

void PlaylistsModelQtAdapter::update_playlist(std::string radio_name, std::vector<std::tuple<std::string, std::string, dmp_library::LibraryEntry> > playlist)
{
	if(radio_name == current_radio) {
		beginResetModel();
	}

	PlaylistsModel::update_playlist(radio_name, playlist);

	if(radio_name == current_radio) {
		endResetModel();
	}
}

void PlaylistsModelQtAdapter::set_current_radio(std::string radio_name)
{
	beginResetModel();
	PlaylistsModel::set_current_radio(radio_name);
	endResetModel();
}
