#pragma once

#include "icu-ascii-transliterator.hpp"

#include "boost/serialization/access.hpp"

#include <iostream>
#include <string>
#include <cstdint>

namespace dmp_library
{

struct LibraryEntry
{
    LibraryEntry() = default;

    LibraryEntry(std::string artist, std::string title, std::string album);

    LibraryEntry(LibraryEntry const&) = default;
    LibraryEntry& operator=(LibraryEntry const&) = delete;
    bool operator==(LibraryEntry const& that) const;

    std::string artist;
    std::string ascii_artist;
    std::string title;
    std::string ascii_title;
    std::string album;
    std::string ascii_album;
    std::uint32_t id;

    friend std::ostream& operator<<(std::ostream& os, LibraryEntry const& le);

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int)
    {
        ar & artist & ascii_artist & title & ascii_title & album & ascii_album & id;
    }

private:
    static uint32_t next_id;
};

}
