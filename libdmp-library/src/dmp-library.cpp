#include "dmp-library.hpp"

#include "taglib/fileref.h"

#include <boost/filesystem.hpp>

#include <vector>

namespace dmp_library {

uint32_t library_entry::next_id = 0;
	
using namespace std;
using namespace boost;

library_entry build_library_entry(filesystem::path p)
{
	TagLib::FileRef file(p.string().c_str());
	TagLib::Tag* t = file.tag();
	if(t) {
		return library_entry(t->artist().toCString(), t->title().toCString(), t->album().toCString(), "4");
	}
	else {
		throw std::runtime_error("No tag data found in: " + p.string());
	}
}

vector<library_entry> build_library(filesystem::recursive_directory_iterator it)
{
	vector<library_entry> library;
	for(; it != filesystem::recursive_directory_iterator(); ++it)
	{
		if(filesystem::is_directory(*it)){
			std::cout << "scanning: " << (*it) << std::endl;
			continue;
		}
		try {
			library.emplace_back(build_library_entry(*it));
		} 
		catch(std::runtime_error const& e) 
		{
			//std::cout << e.what() << std::endl;
			continue;
		}
	}
	return library;
}

vector<library_entry> read_path(string path)
{
	filesystem::path p(path);
	if(filesystem::exists(p))
	{
		if(filesystem::is_regular_file(p))
		{
			return {build_library_entry(p)};
		} else if(filesystem::is_directory(p))
		{
			return build_library(filesystem::recursive_directory_iterator(p));
		} else
		{
			throw runtime_error("unsupported file type: " + p.string() + " is neither a single file or a folder.");
		}
	}
}

ostream& operator<<(ostream& os, library_entry const& le)
{
	return os 	<< "{" << std::endl
				<< "\t" << "UID:" << le.id << std::endl
				<< "\t" << "artist:" << le.artist << std::endl
				<< "\t" << "title:" << le.title << std::endl
				<< "\t" << "album:" << le.album << std::endl
				<< "\t" << "length:" << le.length << std::endl
				<< "}" << std::endl;
}
	
}

