#pragma once

#include "radio_list_model.hpp"

#include <QAbstractListModel>

class RadioListModelQtAdapter : public RadioListModel, public QAbstractListModel
{
public:
	RadioListModelQtAdapter();

	virtual int rowCount(const QModelIndex&) const final;
	virtual QVariant data(const QModelIndex& index, int role) const final;

	virtual void set_radio_names(std::vector<std::string> new_radio_names);
};
