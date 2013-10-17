
#include <dmp-library.hpp>
#include <string>
#include <cassert>
#include <vector>

int main(int argc, char* argv[])
{
    assert(argc == 2);

    std::vector<dmp_library::LibraryEntry> x = dmp_library::create_library(argv[1]);
    for(dmp_library::LibraryEntry const& e : x)
    {
        std::cout << e << std::endl;
    }

    return 0;
}

