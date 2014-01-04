#include "library.hpp"

namespace dmp_library {

Library::Library()
{
}

Library::Library(tracklist_t tracklist, filemap_t filemap)
: tracklist(tracklist)
, filemap(filemap)
{
}

}
