#pragma once

#include "number_pool.hpp"

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/thread/mutex.hpp>
#include <thread>

struct timed_debug {
	static std::string add_call(std::function<void()> f);
	static void remove_call(std::string label);
	
	~timed_debug()
	{
		boost::unique_lock<boost::mutex> l(calls_mutex);
		timed_debug::io_service.stop();
	}
	
private:
	static int interval;
	static NumberPool label_pool;
	static boost::mutex calls_mutex;
	static std::map<std::string, std::function<void()>> calls;
	static boost::asio::io_service io_service;
	static boost::asio::deadline_timer timer;
	
	static void run(unsigned int every_x_seconds);
	
	timed_debug()
	{
		run(interval);
		std::thread([]{io_service.run();}).detach();
	}
	
	static std::unique_ptr<timed_debug> t;
};
