#include "playlists_model_qt_adapter.hpp"

#include "dmp_qt_meta_types.hpp"
#include "boost_any_qvariant_convert.hpp"

PlaylistsModelQtAdapter::PlaylistsModelQtAdapter()
{
}

void PlaylistsModelQtAdapter::set_model(std::shared_ptr<PlaylistsModel> new_model)
{
	model = new_model;
	model->add_delegate<PlaylistUiDelegate>(shared_from_this());
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

Qt::ItemFlags PlaylistsModelQtAdapter::flags(QModelIndex const& index) const
{
	 Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);

	 if (index.isValid())
		 return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
	 else
		 return Qt::ItemIsDropEnabled | defaultFlags;
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

void PlaylistsModelQtAdapter::playlist_update_start(message::PlaylistUpdate update)
{
	if(should_update_view(update.radio_name))
	{
		switch(update.action.type)
		{
			case message::PlaylistUpdate::Action::Type::Append:
			{
				beginInsertRows(QModelIndex(), model->row_count(), update.playlist.size() - 1);
				break;
			}
			case message::PlaylistUpdate::Action::Type::Update:
			{
				beginResetModel();
				break;
			}
			case message::PlaylistUpdate::Action::Type::Insert:
			{
				break;
			}
			case message::PlaylistUpdate::Action::Type::Move:
			{
				break;
			}
			case message::PlaylistUpdate::Action::Type::Reset:
			{
				beginResetModel();
				break;
			}
			case message::PlaylistUpdate::Action::Type::NoAction:
			default:
			{
				throw std::runtime_error("This should never happen");
			}
		}
	}
}

void PlaylistsModelQtAdapter::playlist_update_end(message::PlaylistUpdate update)
{
	if(should_update_view(update.radio_name))
	{
		switch(update.action.type)
		{
			case message::PlaylistUpdate::Action::Type::Append:
			{
				endInsertRows();
				break;
			}
			case message::PlaylistUpdate::Action::Type::Update:
			{
				endResetModel();
				break;
			}
			case message::PlaylistUpdate::Action::Type::Insert:
			{
				break;
			}
			case message::PlaylistUpdate::Action::Type::Move:
			{
				break;
			}
			case message::PlaylistUpdate::Action::Type::Reset:
			{
				endResetModel();
				break;
			}
			case message::PlaylistUpdate::Action::Type::NoAction:
			default:
			{
				throw std::runtime_error("This should never happen");
			}
		}
	}
}

void PlaylistsModelQtAdapter::current_radio_change_start()
{
	beginResetModel();
}

void PlaylistsModelQtAdapter::current_radio_change_end()
{
	endResetModel();
}

void PlaylistsModelQtAdapter::set_radios_start()
{
	beginResetModel();
}

void PlaylistsModelQtAdapter::set_radios_end()
{
	endResetModel();
}

void PlaylistsModelQtAdapter::remove_radio_start()
{
	beginResetModel();
}

void PlaylistsModelQtAdapter::remove_radio_end()
{
	endResetModel();
}
