#pragma once

#include "icu-ascii-transliterator.hpp"

#include "boost/serialization/access.hpp"

#include <iostream>
#include <string>
#include <cstdint>

namespace dmp_library
{

struct library_entry
{
    library_entry() = default;

    library_entry(std::string artist, std::string title, std::string album);

    library_entry& operator=(library_entry const&) = delete;
    bool operator==(library_entry const& that) const;

    std::string artist;
    std::string ascii_artist;
    std::string title;
    std::string ascii_title;
    std::string album;
    std::string ascii_album;
    std::uint32_t id;

    friend std::ostream& operator<<(std::ostream& os, library_entry const& le);

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int)
    {
        ar & artist & ascii_artist & title & ascii_title & album & ascii_album & id;
    }

private:
    static uint32_t next_id;
};

}
