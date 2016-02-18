
#include "time_util.hpp"

#include <boost/date_time/gregorian_calendar.hpp>
#include <boost/date_time/posix_time/posix_time_config.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/iterator/iterator_traits.hpp>

#include <sstream>

std::string get_current_time()
{
	namespace pt = boost::posix_time;
	
	static std::ostringstream msg;
	static const std::locale locale(msg.getloc());
	static auto f = std::make_unique<pt::time_facet>("%H:%M:%S%F");
	
	const pt::ptime now = pt::microsec_clock::local_time();
	msg.imbue(std::locale(msg.getloc(),f.get()));
	msg << now;
	std::string str = msg.str();
	msg.str("");
	msg.clear();
	
	return str;
}
