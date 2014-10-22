#include "call_event.hpp"

CallEvent::CallEvent(std::function<void()> function)
: QEvent(QEvent::Type::User)
, function(function)
{}

void CallEvent::operator()()
{
	function();
}
