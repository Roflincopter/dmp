#include "library.hpp"

namespace dmp_library {

Library::Library()
{
}

Library::Library(boost::filesystem::path path, tracklist_t tracklist, filemap_t filemap)
: path(path)
, tracklist(tracklist)
, filemap(filemap)
{
}

std::string Library::get_filename(LibraryEntry entry) const
{
	return filemap.at(entry.id);
}

}
