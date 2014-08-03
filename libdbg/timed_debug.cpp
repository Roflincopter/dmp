
#include "timed_debug.hpp"

int timed_debug::interval(5);
int timed_debug::next_label(0);
boost::mutex timed_debug::calls_mutex;
boost::asio::io_service timed_debug::io_service;
boost::asio::deadline_timer timed_debug::timer(timed_debug::io_service);
std::map<std::string, std::function<void()>> timed_debug::calls;

std::string timed_debug::add_call(std::function<void ()> f)
{
	boost::unique_lock<boost::mutex> l(calls_mutex);
	
	std::string label = std::to_string(next_label++);
	
	calls[label] = f;
	return label;
}

void timed_debug::remove_call(std::string label)
{
	boost::unique_lock<boost::mutex> l(calls_mutex);
	
	auto const it = calls.find(label);
	if(it != calls.cend()) {
		calls.erase(it);
	} else {
		throw std::runtime_error("timed_debug::remove_call called with invalid label");
	}
}

void timed_debug::run(unsigned int every_x_seconds)
{
	timer.expires_from_now(boost::posix_time::seconds(every_x_seconds));
	
	auto cb = [=](boost::system::error_code ec){
		if(ec)
		{
			throw std::runtime_error("something went wrong in timed_debug::run.");
		}
		
		boost::unique_lock<boost::mutex> l(calls_mutex);

		for(auto&& p : calls) {
			p.second();
		}
		
		timed_debug::run(every_x_seconds);
	};
	
	timer.async_wait(cb);
}

std::unique_ptr<timed_debug> timed_debug::t(new timed_debug());
