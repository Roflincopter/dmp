#pragma once

#include <QModelIndex>
#include <QItemSelectionModel>

#include <memory>

template <typename T>
struct QtAdapter
{
	std::shared_ptr<const T> model;
	
	QtAdapter()
	: model(std::make_shared<const T>())
	{}
	
	void set_model(std::shared_ptr<const T> new_model)
	{
		model = new_model;
	}
	
	std::shared_ptr<T> get_model()
	{
		return model;
	}
};
