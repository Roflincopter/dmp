
#include <dmp-library.hpp>
#include <string>
#include <cassert>
#include <vector>

int main(int argc, char* argv[])
{
	assert(argc == 2);

	dmp_library::Library x = dmp_library::create_library(argv[1]);
	for(dmp_library::LibraryEntry const& e : x.tracklist)
	{
		std::cout << e << std::endl;
	}

	return 0;
}

