#include "dmp-library.hpp"

int main()
{
	dmp_library::parse_query("artist contains \"x\" or title is \"y\" and not (not artist is \"y\")");
}