#pragma once

#include <string>
#include <vector>

namespace dmp_library {

    std::string transliterate_to_ascii(std::string const& str);
    std::vector<std::string> possible_transliterator_ids();
}