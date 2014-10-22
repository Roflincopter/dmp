#pragma once

#include <QEvent>

#include <functional>

class CallEvent : public QEvent
{
	std::function<void()> function;

public:
	CallEvent(std::function<void()> function);
	
	void operator()();
};
