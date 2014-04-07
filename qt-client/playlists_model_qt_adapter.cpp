#include "playlists_model_qt_adapter.hpp"

#include "boost_any_to_qvariant.hpp"

PlaylistsModelQtAdapter::PlaylistsModelQtAdapter()
: model(std::make_shared<PlaylistsModel>())
{
}

void PlaylistsModelQtAdapter::set_model(std::shared_ptr<PlaylistsModel> new_model)
{
	model = new_model;
}

int PlaylistsModelQtAdapter::rowCount(QModelIndex const&) const
{
	return model->row_count();
}

int PlaylistsModelQtAdapter::columnCount(QModelIndex const&) const
{
	return model->column_count();
}

QVariant PlaylistsModelQtAdapter::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation == Qt::Vertical) {
		return QVariant();
	}

	if(role != Qt::DisplayRole) {
		return QVariant();
	}

	return QVariant(QString::fromStdString(model->header_data(section)));
}

QVariant PlaylistsModelQtAdapter::data(QModelIndex const& index, int role) const
{
	if(role != Qt::DisplayRole) {
		return QVariant();
	}

	return to_qvariant<PlaylistsModel::ElementType>(model->get_cell(index.row(), index.column()), index.column());
}

bool PlaylistsModelQtAdapter::should_update_view(std::string radio_name)
{
	return radio_name == model->get_current_radio();
}

void PlaylistsModelQtAdapter::update(std::string radio_name, Playlist playlist)
{
	if(should_update_view(radio_name)) {
		beginResetModel();
	}

	model->update(radio_name, playlist);

	if(should_update_view(radio_name)) {
		endResetModel();
	}
}

void PlaylistsModelQtAdapter::append(std::string radio_name, Playlist playlist)
{
	if(should_update_view(radio_name)) {
		beginInsertRows(QModelIndex(), rowCount(), rowCount() + playlist.size() - 1);
	}

	model->append(radio_name, playlist);

	if(should_update_view(radio_name)) {
		endInsertRows();
	}
}

void PlaylistsModelQtAdapter::reset(std::string radio_name)
{
	if(should_update_view(radio_name)) {
		beginResetModel();
	}

	model->reset(radio_name);

	if(should_update_view(radio_name)) {
		endResetModel();
	}
}

void PlaylistsModelQtAdapter::create_radio(std::string radio_name)
{
	model->create_radio(radio_name);
}

void PlaylistsModelQtAdapter::set_current_radio(std::string radio_name)
{
	beginResetModel();
	model->set_current_radio(radio_name);
	endResetModel();
}
