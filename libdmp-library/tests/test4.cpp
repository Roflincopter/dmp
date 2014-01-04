
#include <dmp-library.hpp>
#include <string>
#include <cassert>
#include <vector>

int main(int argc, char* argv[])
{
	assert(argc == 2);

	dmp_library::Library l1 = dmp_library::create_library(argv[1], false);
	dmp_library::Library l2 = dmp_library::create_library(argv[1], true);

	return
		!std::equal(l1.tracklist.begin(), l1.tracklist.end(), l2.tracklist.begin()) &&
		!std::equal(l1.filemap.begin(), l1.filemap.end(), l2.filemap.begin());
}
