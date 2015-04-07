
#include <dmp-library.hpp>
#include <string>
#include <cassert>
#include <vector>

int main(int argc, char* argv[])
{
	if(argc != 2) {
		throw std::runtime_error("this test requires 1 path to a music directory");
	}

	dmp_library::LibraryFolder x = dmp_library::create_library(argv[1]);
	for(dmp_library::LibraryEntry const& e : x.tracklist)
	{
		std::cout << e << std::endl;
	}

	return 0;
}

