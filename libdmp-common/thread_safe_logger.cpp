#include "thread_safe_logger.hpp"

ThreadSafeLogger::ThreadSafeLogger(std::string filename)
: mutex(std::make_unique<std::mutex>())
, file(filename)
{}

ThreadSafeLogger::ThreadSafeLogger(ThreadSafeLogger&& that)
: mutex(std::move(that.mutex))
, file(std::move(that.file))
{}
