#include "library.hpp"

#include "icu-ascii-transliterator.hpp"

#include "dmp_config.hpp"
#include "archive.hpp"

#include <taglib/audioproperties.h>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/tstring.h>

#include <boost/filesystem/operations.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <stdexcept>
#include <exception>
#include <fstream>

namespace dmp_library {

Library::Library()
: load_info(std::make_shared<LoadInfo>())
, library()
{}

Library::Library(library_t library)
: load_info(std::make_shared<LoadInfo>())
, library(library)
{
}

Library Library::read_cache(std::string cache_path){
	std::ifstream ifs(cache_path);
	if(ifs)
	{
		load_info->cache_file = cache_path;
		load_info->reading_cache = true;
		IArchive ar(ifs);
		Library lib;
		ar & lib;
		load_info->reading_cache = false;
		return lib;
	}
	else
	{
		throw std::runtime_error("opening file " + cache_path + " for reading failed: make this a nice exception.");
	}
}

void Library::write_cache(const std::string& cache_path)
{
	std::ofstream ofs(cache_path);
	if(ofs)
	{
		OArchive ar(ofs);
		ar & library;
	}
	else
	{
		std::cout << "opening file " + cache_path + " for writing failed: make this a nice exception." << std::endl;
	}
	return;
}

boost::optional<LibraryEntry> Library::build_library_entry(boost::filesystem::path p)
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
		//gcc 4.9 semantic analysis will report control reaches end of non-void function without this return statement
		//clang 3.6 will report 'return' will never be executed
#ifndef __clang__
		return boost::optional<LibraryEntry>();
#endif
	}
	else {
		std::cout << "fileref or audio_prop not valid for file: " << p.string() << std::endl;
		return boost::optional<LibraryEntry>();
	}
}

Library Library::build_library(boost::filesystem::path p)
{
	library_t lib;
	
	load_info->current_track = 1;
	load_info->current_max_tracks = std::distance(
		boost::filesystem::recursive_directory_iterator(p),
		boost::filesystem::recursive_directory_iterator()
	);

	for(auto it = boost::filesystem::recursive_directory_iterator(p); it != boost::filesystem::recursive_directory_iterator(); ++it)
	{
		if(load_info->should_stop) {
			throw Interrupted();
		}
		load_info->current_track++;
		if(boost::filesystem::is_directory(*it)){
			continue;
		}
		try {
			
			boost::optional<LibraryEntry> entry = build_library_entry(*it);
			if(entry) {
				lib.insert(std::make_pair(entry.get(), EntryLocation{entry.get(), it->path().string()}));
			}
		}
		catch(std::exception&)
		{
			continue;
		}
	}
	return Library(lib);
}

Library Library::parse_directory(const std::string &directory_path)
{
	boost::filesystem::path p(directory_path);
	if(boost::filesystem::exists(p))
	{
		if(boost::filesystem::is_directory(p))
		{
			return build_library(p);
		} else
		{
			throw std::runtime_error("unsupported file type: " + p.string() + " is not a folder.");
		}
	}
	else
	{
		throw std::runtime_error("File/Folder: \"" + p.string() + "\" does not exist.");
	}
}

Library Library::create(config::LibraryInfo info, bool use_cache, bool create_cache) {
	boost::filesystem::path cache_file = config::get_library_folder_name() / info.cache_file;

	if(boost::filesystem::exists(cache_file) && use_cache)
	{
		Library lib;
		lib = read_cache(cache_file.string());
		return lib;
	}
	else
	{
		Library lib = parse_directory(info.path);

		if(lib.library.empty()) {
			std::cout << "library was empty when writing cache file." << std::endl;
		}

		if(create_cache)
		{
			lib.write_cache(cache_file.string());
		}
		return lib;
	}
}

std::string Library::get_filename(LibraryEntry const& entry) const
{
	if(library.count(entry) == 1) {
		return library.find(entry)->second.location;
	} else {
		auto pair = library.equal_range(entry);
		for(auto it = pair.first; it != pair.second; ++it) {
			if(it->second.entry == entry) {
				return it->second.location;
			}
		}
	}
	throw EntryNotFound();
}

void Library::clear() {
	library.clear();
	load_info = std::make_shared<LoadInfo>();
}

void Library::add_folder(config::LibraryInfo const& info)
{
	auto sublib = create(info);
	library.insert(sublib.library.begin(), sublib.library.end());
}

void Library::load_library(const std::vector<config::LibraryInfo>& library_info) {
	load_info->nr_folders = library_info.size();
	for(size_t i = 0; i < library_info.size(); ++i) {
		load_info->current_folder = i + 1;
		
		auto&& entry = library_info[i];
		add_folder(entry);
	}
}

EntryNotFound::EntryNotFound()
: std::runtime_error("Entry not found in this library")
{}

}
