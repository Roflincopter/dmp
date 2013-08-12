#include "dmp-library.hpp"

#include "taglib/fileref.h"

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

#include <vector>

namespace dmp_library {

using namespace std;
using namespace boost;

boost::optional<library_entry> build_library_entry(filesystem::path p)
{
    TagLib::FileRef file(p.string().c_str());
    TagLib::Tag* t = file.tag();
    if(t) {
        return boost::optional<library_entry>(library_entry(t->artist().to8Bit(true), t->title().to8Bit(true), t->album().to8Bit(true)));
    }
    else {
        return boost::optional<library_entry>();
    }
}

vector<library_entry> build_library(filesystem::recursive_directory_iterator it)
{
    vector<library_entry> library;
    for(; it != filesystem::recursive_directory_iterator(); ++it)
    {
        if(filesystem::is_directory(*it)){
            continue;
        }
        try {
            boost::optional<library_entry> entry = build_library_entry(*it);
            if(entry) {
                library.emplace_back(entry.get());
            }
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
            boost::optional<library_entry> entry = build_library_entry(p);
            if(entry) {
                return {entry.get()};
            } else {
                return {};
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
        throw runtime_error("File/Folder: \"" + path + "\" does not exist.");
    }
}

}

