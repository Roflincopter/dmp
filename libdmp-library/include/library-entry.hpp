#pragma once

#include "icu-ascii-transliterator.hpp"

#include <iostream>
#include <string>
#include <inttypes.h>

namespace dmp_library
{



struct library_entry
{
    library_entry(std::string artist, std::string title, std::string album);

    library_entry& operator=(library_entry const&) = delete;

    std::string artist;
    std::string ascii_artist;
    std::string title;
    std::string ascii_title;
    std::string album;
    std::string ascii_album;
    uint32_t id;

    friend std::ostream& operator<<(std::ostream& os, library_entry const& le);

private:
    static uint32_t next_id;
};

}