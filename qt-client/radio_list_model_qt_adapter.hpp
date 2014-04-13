#pragma once

#include "qt_adapter.hpp"
#include "radio_list_model.hpp"
#include "dmp_client_ui_delegate.hpp"

#include <QAbstractListModel>

#include <memory>

class RadioListModelQtAdapter : public DmpClientUiDelegate, public QtAdapter<RadioListModel>, public QAbstractListModel
{
public:
	RadioListModelQtAdapter();
	
	void set_model(std::shared_ptr<RadioListModel> new_model);

	virtual int rowCount(const QModelIndex&) const override final;
	virtual QVariant data(const QModelIndex& index, int role) const override final;

	virtual void set_radios_start() override final;
	virtual void set_radios_end() override final;
	
	virtual void add_radio_start() override final;
	virtual void add_radio_end() override final;
};
