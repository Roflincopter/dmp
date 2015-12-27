#pragma once

#include "qt_adapter.hpp"
#include "playlist_ui_delegate.hpp"

#include <QAbstractItemModel>
#include <QVariant>

#include <memory>
#include <string>

class PlaylistsModel;
namespace message { struct PlaylistUpdate; }

class PlaylistsModelQtAdapter : public PlaylistUiDelegate, public std::enable_shared_from_this<PlaylistsModelQtAdapter>, public QtAdapter<PlaylistsModel>, public QAbstractTableModel
{
private:
	bool should_update_view(std::string radio_name);
public:
	PlaylistsModelQtAdapter();
	
	void set_model(std::shared_ptr<PlaylistsModel> model);

	int rowCount(const QModelIndex & = QModelIndex()) const override final;
	int columnCount(const QModelIndex &) const override final;
	virtual QVariant data(const QModelIndex &index, int role) const override final;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override final;

	virtual bool setData(QModelIndex const& index, QVariant const& value, int role) override final;

	virtual Qt::ItemFlags flags(QModelIndex const& index) const override final;

	virtual void playlist_update_start(message::PlaylistUpdate update) override final;
	virtual void playlist_update_end(message::PlaylistUpdate update) override final;

	virtual void clear_start() override final;
	virtual void clear_end() override final;

	virtual void current_radio_change_start() override final;
	virtual void current_radio_change_end() override final;
	
	virtual void set_radios_start() override final;
	virtual void set_radios_end() override final;

	virtual void remove_radio_start() override final;
	virtual void remove_radio_end() override final;
};
