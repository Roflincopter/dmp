#pragma once

#include <memory>

template <typename T>
struct QtAdapter
{
	std::shared_ptr<T> model;
	
	QtAdapter()
	: model(std::make_shared<T>())
	{}
	
	void set_model(std::shared_ptr<T> new_model)
	{
		model = new_model;
	}
	
	std::shared_ptr<T> get_model()
	{
		return model;
	}
};
