
#include <dmp-library.hpp>
#include <string>
#include <cassert>
#include <vector>

int main(int argc, char* argv[])
{
	assert(argc == 2);
	std::vector<dmp_library::library_entry> x = dmp_library::read_path(argv[1]);
	for(dmp_library::library_entry const& e : x)
	{
		std::cout << e << std::endl;
	}
	return 0;
}

