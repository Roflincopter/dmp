#include "library.hpp"

namespace dmp_library {

LibraryFolder::LibraryFolder()
{
}

LibraryFolder::LibraryFolder(boost::filesystem::path path, tracklist_t tracklist, filemap_t filemap)
: path(path)
, tracklist(tracklist)
, filemap(filemap)
{
}

LibraryFolder LibraryFolder::read_cache(std::string cache_path){
	std::ifstream ifs(cache_path);
	if(ifs)
	{
		boost::archive::text_iarchive ar(ifs);
		LibraryFolder lib;
		ar & lib;
		return lib;
	}
	else
	{
		throw std::runtime_error("opening file " + cache_path + " for reading failed: make this a nice exception.");
	}
}

boost::optional<LibraryEntry> LibraryFolder::build_library_entry(boost::filesystem::path p)
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

LibraryFolder LibraryFolder::build_library(boost::filesystem::path p)
{
	std::vector<LibraryEntry> library;
	std::map<uint32_t, std::string> filemap;

	for(auto it = boost::filesystem::recursive_directory_iterator(p); it != boost::filesystem::recursive_directory_iterator(); ++it)
	{
		if(boost::filesystem::is_directory(*it)){
			continue;
		}
		try {
			boost::optional<LibraryEntry> entry = build_library_entry(*it);
			if(entry) {
				library.emplace_back(entry.get());
				filemap.emplace(entry.get().id, (*it).path().string());
			}
		}
		catch(std::exception&)
		{
			continue;
		}
	}
	return LibraryFolder(p, library, filemap);
}

LibraryFolder LibraryFolder::parse_directory(const std::string &directory_path)
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

LibraryFolder LibraryFolder::create(config::LibraryInfo info, bool use_cache, bool create_cache) {
	boost::filesystem::path cache_file = config::get_library_folder_name() / info.cache_file;

	if(boost::filesystem::exists(cache_file) && use_cache)
	{
		LibraryFolder lib = read_cache(cache_file.string());
		return lib;
	}
	else
	{
		LibraryFolder lib = parse_directory(info.path);

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

std::string Library::get_filename(uint32_t folder_id, LibraryEntry entry) const
{
	return library.at(folder_id).filemap.at(entry.id);
}

void Library::clear() {
	library.clear();
}

void Library::add_folder(config::LibraryInfo info)
{
	library[library.size()] = LibraryFolder::create(info);
}

}
