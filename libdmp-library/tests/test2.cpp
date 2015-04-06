
#include "dmp-library.hpp"

#include <cassert>

int main(int argc, char* argv[])
{
	if(argc != 2) {
		throw std::runtime_error("This test requires 1 path to a music directory as argument");
	}
	std::string dir(argv[1]);

	auto database = dmp_library::create_library(dir, false);
	dmp_library::LibrarySearcher searcher(database);

	std::shared_ptr<dmp_library::Query> q = dmp_library::parse_query("artist contains \"x\" and not title contains \"x\" or album is \"x\"");
	searcher.search(q);
	std::cout << std::endl;

	/*
	auto result_vec = searcher.search(q);
	for(size_t x : result_vec)
	{
		std::cout << database[x] << std::endl;
		std::cin.ignore();
	}
	*/
}
