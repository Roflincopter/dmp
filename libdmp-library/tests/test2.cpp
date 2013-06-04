
#include "dmp-library.hpp"

#include <cassert>

int main(int argc, char* argv[])
{
	assert(argc == 2);
	std::string dir(argv[1]);
	
	auto database = dmp_library::read_path(dir);
	auto searcher = dmp_library::library_search(database);
	
	dmp_library::parse_query("not artist contains \"lolol\"");
	/*
	auto result_vec = searcher.search(q);
	for(size_t x : result_vec)
	{
		std::cout << database[x] << std::endl;
		std::cin.ignore();
	}
	*/
}