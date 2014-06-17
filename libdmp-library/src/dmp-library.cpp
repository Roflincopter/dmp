#include "dmp-library.hpp"
#include "icu-ascii-transliterator.hpp"

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

Library build_library(filesystem::recursive_directory_iterator it)
{
	vector<LibraryEntry> library;
	std::map<uint32_t, std::string> filemap;

	for(; it != filesystem::recursive_directory_iterator(); ++it)
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
	return Library(library, filemap);
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
		if(filesystem::is_regular_file(p))
		{
			boost::optional<LibraryEntry> entry = build_library_entry(p);
			if(entry) {
				return Library({entry.get()},{{entry.get().id, p.string()}});
			} else {
				return Library();
			}
		} else if(filesystem::is_directory(p))
		{
			return build_library(filesystem::recursive_directory_iterator(p));
		} else
		{
			throw runtime_error("unsupported file type: " + p.string() + " is neither a single file or a folder.");
		}
	}
	else
	{
		throw runtime_error("File/Folder: \"" + p.string() + "\" does not exist.");
	}
}

Library create_library(string path, bool use_cache, bool create_cache)
{
	filesystem::path cache_path = filesystem::path(path) / filesystem::path(cache_file);

	if(filesystem::exists(cache_path) && use_cache)
	{
		Library lib = read_cache(cache_path.string());
		write_cache(cache_path.string(), lib);
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
			write_cache(cache_path.string(), lib);
		}
		return lib;
	}
}

}

