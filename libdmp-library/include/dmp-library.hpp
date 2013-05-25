#pragma once

#include <iostream>
#include <string>
#include <inttypes.h>
#include <vector>

namespace dmp_library
{

struct library_entry 
{	
	library_entry(std::string artist, std::string title, std::string album)
	: artist(artist)
	, title(title)
	, album(album)
	, id(next_id++)
	{}
	
	library_entry& operator=(library_entry const&) = delete;
	
	std::string artist;
	std::string title;
	std::string album;
	uint32_t id;
	
	friend std::ostream& operator<<(std::ostream& os, library_entry const& le);
	
private:
	static uint32_t next_id;
};

std::vector<library_entry> read_path(std::string path);	

}