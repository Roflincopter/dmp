#pragma once

#include <mutex>
#include <fstream>
#include <memory>

class ThreadSafeLogger
{
	std::unique_ptr<std::mutex> mutex;
	std::ofstream file;

	typedef std::ostream& (*manip)(std::ostream&);

public:
	ThreadSafeLogger(std::string filename);
	ThreadSafeLogger(ThreadSafeLogger&& that);
	
	template <typename T>
	ThreadSafeLogger& operator<<(T const& rh) {
		std::unique_lock<std::mutex> l(*mutex);
		file << rh;
		return *this;
	}
	
	ThreadSafeLogger& operator<<(manip pf)
	{
		std::unique_lock<std::mutex> l(*mutex);
		file << pf;
		return *this;
	}
	
};