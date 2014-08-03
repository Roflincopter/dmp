
#include "time_util.hpp"

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
