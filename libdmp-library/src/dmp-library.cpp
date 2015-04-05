#include "dmp-library.hpp"
#include "icu-ascii-transliterator.hpp"

#include "dmp_config.hpp"

#include <taglib/fileref.h>
#include <taglib/tag.h>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <vector>
#include <fstream>

namespace dmp_library {

using namespace std;
using namespace boost;

boost::optional<LibraryEntry> build_library_entry(filesystem::path p)
{
	TagLib::FileRef file(p.string().c_str(), true, TagLib::AudioProperties::ReadStyle::Accurate);
	TagLib::AudioProperties* audio_prop = file.audioProperties();
	TagLib::Tag* t = file.tag();
	if(t && audio_prop) {
		try {
			return boost::optional<LibraryEntry>(LibraryEntry(t->artist().to8Bit(true), t->title().to8Bit(true), t->album().to8Bit(true), t->track(), audio_prop->length()));
		} catch (std::exception& e) {
			std::cout << "Creating LibraryEntry failed: " << e.what() << std::endl;
			for (auto&& id : possible_transliterator_ids()) {
				std::cout << id << std::endl;
			}
			
			return boost::optional<LibraryEntry>();
		}
	}
	else {
		std::cout << "fileref or audio_prop not valid for file: " << p.string() << std::endl;
		return boost::optional<LibraryEntry>();
	}
	return boost::optional<LibraryEntry>();
}

Library build_library(filesystem::path p)
{
	vector<LibraryEntry> library;
	std::map<uint32_t, std::string> filemap;

	for(auto it = filesystem::recursive_directory_iterator(p); it != filesystem::recursive_directory_iterator(); ++it)
	{
		if(filesystem::is_directory(*it)){
			continue;
		}
		try {
			boost::optional<LibraryEntry> entry = build_library_entry(*it);
			if(entry) {
				library.emplace_back(entry.get());
				filemap.emplace(entry.get().id, (*it).path().string());
			}
		}
		catch(std::exception &e)
		{
			continue;
		}
	}
	return Library(p, library, filemap);
}

Library read_cache(std::string const& cache_path)
{
	std::ifstream ifs(cache_path);
	if(ifs)
	{
		archive::text_iarchive ar(ifs);
		Library lib;
		ar & lib;
		return lib;
	}
	else
	{
		throw std::runtime_error("opening file " + cache_path + " for reading failed: make this a nice exception.");
	}
}

void write_cache(std::string const& cache_path, Library const& lib)
{
	std::ofstream ofs(cache_path);
	if(ofs)
	{
		archive::text_oarchive ar(ofs);
		ar & lib;
	}
	else
	{
		std::cout << "opening file " + cache_path + " for writing failed: make this a nice exception." << std::endl;
	}
	return;
}

Library parse_directory(std::string const& directory_path)
{
	filesystem::path p(directory_path);
	if(filesystem::exists(p))
	{
		if(filesystem::is_directory(p))
		{
			return build_library(p);
		} else
		{
			throw runtime_error("unsupported file type: " + p.string() + " is not a folder.");
		}
	}
	else
	{
		throw runtime_error("File/Folder: \"" + p.string() + "\" does not exist.");
	}
}

Library create_library(string path, bool use_cache, bool create_cache)
{
	//check if this folder has already been indexed.
	bool found = false;
	string cache_file_name;
	for(auto&& entry : config::get_library_information()) {
		if(entry.path == path) {
			found = true;
			cache_file_name = entry.cache_file;
		}
	}

	boost::filesystem::path cache_file;
	if(found) {
		//if so set the correct cache file
		cache_file = cache_folder / cache_file_name;
	} else {
		//else make a unique cache file
		do {
			cache_file = cache_folder / boost::filesystem::unique_path("%%%%%%%%%%");
		} while(boost::filesystem::exists(cache_file));

		config::add_library(
			boost::filesystem::path(path).filename().string(),
			path,
			cache_file.filename().string()
		);
	}

	if(filesystem::exists(cache_file) && use_cache)
	{
		Library lib = read_cache(cache_file.string());
		return lib;
	}
	else
	{
		Library lib = parse_directory(path);
		
		if(lib.tracklist.empty()) {
			std::cout << "library was empty when writing cache file." << std::endl;
		}
		
		if(create_cache)
		{
			write_cache(cache_file.string(), lib);
		}
		return lib;
	}
}

}

