#include "dmp_client_gui_playlists.hpp"

#include <QHeaderView>

DmpClientGuiPlaylists::DmpClientGuiPlaylists(QWidget *parent)
: QTableView(parent)
, client(nullptr)
, model(std::make_shared<PlaylistsModelQtAdapter>())
, delegate()
{
	setModel(model.get());
	setItemDelegate(&delegate);
	hideColumn(1);
	hideColumn(3);
	hideColumn(5);
	horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void DmpClientGuiPlaylists::set_client(std::shared_ptr<DmpClientInterface> new_client)
{
	client = new_client;
	model->set_model(client->get_playlists_model());
	client->get_playlists_model()->add_delegate<PlaylistUiDelegate>(shared_from_this());
	client->get_playlists_model()->add_delegate<PlaylistUiDelegate>(model);
}

void DmpClientGuiPlaylists::currentRadioChanged(std::string name)
{
	client->set_current_radio(name);
}

void DmpClientGuiPlaylists::deleteSelected()
{
	auto&& selection = this->selectionModel()->selectedRows();
	std::vector<PlaylistId> ids;
	auto radio = model->model->get_current_radio();
	for(auto&& index : selection) {
		auto playlist_id = model->model->get_playlist_id(index.row());
		
		ids.push_back(playlist_id);
	}
	client->unqueue(radio, ids);
}

void DmpClientGuiPlaylists::move_selected(bool up)
{
	auto&& selection = this->selectionModel()->selectedRows();
	std::vector<uint32_t> selected;
	auto radio = model->model->get_current_radio();
	for(auto&& index : selection) {
		auto playlist_id = model->model->get_playlist_id(index.row());
		selected.push_back(playlist_id);
	}
	client->move_queue(radio, selected, up);
}

void DmpClientGuiPlaylists::moveSelectedUp()
{
	move_selected(true);
}

void DmpClientGuiPlaylists::moveSelectedDown()
{
	move_selected(false);
}
