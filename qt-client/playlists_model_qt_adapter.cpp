#include "playlists_model_qt_adapter.hpp"

#include "boost_any_to_qvariant.hpp"

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

	return to_qvariant<PlaylistsModel::ElementType>(get_cell(index.row(), index.column()), index.column());
}

void PlaylistsModelQtAdapter::update(std::string radio_name, Playlist playlist)
{
	if(radio_name == current_radio) {
		beginResetModel();
	}

	PlaylistsModel::update(radio_name, playlist);

	if(radio_name == current_radio) {
		endResetModel();
	}
}

void PlaylistsModelQtAdapter::append(std::string radio_name, Playlist playlist)
{
	if(radio_name == current_radio) {
		beginInsertRows(QModelIndex(), rowCount(), rowCount() + playlist.size() - 1);
	}

	PlaylistsModel::append(radio_name, playlist);

	if(radio_name == current_radio) {
		endInsertRows();
	}
}

void PlaylistsModelQtAdapter::reset(std::string radio_name)
{
	if(radio_name == current_radio) {
		beginResetModel();
	}

	PlaylistsModel::reset(radio_name);

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
