
#include "time_util.hpp"

#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/posix_time/posix_time_config.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/iterator/iterator_traits.hpp>

#include <sstream>

std::string get_current_time()
{
	namespace pt = boost::posix_time;

	std::ostringstream msg;
	const pt::ptime now = pt::microsec_clock::local_time();
	pt::time_facet*const f = new pt::time_facet("%H:%M:%S%F");
	msg.imbue(std::locale(msg.getloc(),f));
	msg << now;
	return msg.str();
}
