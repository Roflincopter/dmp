#pragma once

#include "qt_adapter.hpp"
#include "radio_list_ui_delegate.hpp"

#include <QAbstractItemModel>
#include <QVariant>

#include <memory>

class RadioListModel;

class RadioListModelQtAdapter : public QtAdapter<RadioListModel>, public QAbstractListModel, public RadioListUiDelegate, public std::enable_shared_from_this<RadioListUiDelegate>
{
public:
	RadioListModelQtAdapter();
	
	void set_model(std::shared_ptr<RadioListModel> new_model);

	virtual int rowCount(const QModelIndex&) const override final;
	virtual QVariant data(const QModelIndex& index, int role) const override final;

	QModelIndex get_model_index_for(int row);
	
	virtual void set_radios_start() override final;
	virtual void set_radios_end() override final;
	
	virtual void clear_start() override final;
	virtual void clear_end() override final;
	
	virtual void add_radio_start() override final;
	virtual void add_radio_end() override final;

	virtual void remove_radio_start() override final;
	virtual void remove_radio_end() override final;
};
