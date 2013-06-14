#include "dmp-library.hpp"

int main()
{
	dmp_library::parse_query("artist contains \"x\" or title is \"y\" and not (not artist is \"y\") f");
	dmp_library::parse_query("artist contains \"x\" or title is \"y\" and (not artist is \"y\")");
	dmp_library::parse_query("artist contains \"x\" or title is \"y\" or (not artist is \"y\")");
	dmp_library::parse_query("artist contains \"x\" or title is \"y\" and not (not artist is \"y\"");
	dmp_library::parse_query("artist contains x\" or title is \"y\" and not (not artist is \"y\")");
	dmp_library::parse_query("artist contains \"x\" or titles is \"y\" and not (not artist is \"y\")");
	dmp_library::parse_query("artist contains \"x\" or tilte is \"y\" and not (not artist is \"y\")");
}