#include "dmp-library.hpp"

#include <stdexcept>

int main()
{
	std::vector<std::string> queries{
		"artist contains \"x\"",
		"artist contains \"x\" or title is \"y\" or album is \"z\"",
		"artist contains \"x\" or title is \"y\" and not (not artist is \"y\") f",
		"artist contains \"x\" or title is \"y\" and (not artist is \"y\")",
		"artist contains \"x\" or title is \"y\" or (not artist is \"y\")",
		"artist contains \"x\" or title is \"y\" and not (not artist is \"y\"",
		"artist contains x\" or title is \"y\" and not (not artist is \"y\")",
		"artist contains \"x or title is \"y\" and not (not artist is \"y\")",
		"artist contains \"x\" or titles is \"y\" and not (not artist is \"y\")",
		"artist contains \"x\" or tilte is \"y\" and not (not artist is \"y\")"
	};

	for(std::string query : queries)
	{
		try {
			dmp_library::parse_query(query);
		} catch (dmp_library::ParseError &err) {
			std::cout << err.what() << std::endl;
			std::cout << query << std::endl << std::endl;
			continue;
		}
		std::cout << "parse succeeded: " << std::endl;
		std::cout << query << std::endl << std::endl;
	}
}
