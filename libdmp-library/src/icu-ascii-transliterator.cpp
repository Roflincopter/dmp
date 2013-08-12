#include "icu-ascii-transliterator.hpp"

#include <unicode/translit.h>
#include <unicode/unistr.h>

#include <stdexcept>
#include <iostream>

using namespace std;
using namespace icu;
namespace dmp_library {


string transliterate_to_ascii(string const& x)
{
    UErrorCode ec;

    string id1("Any-Latin");
    UnicodeString u_id1(id1.c_str());
    auto transliterator1 = Transliterator::createInstance(u_id1, UTRANS_FORWARD, ec);

    if(U_FAILURE(ec)) {
        throw runtime_error("could not create a transliterator instance: " + id1);
    }

    string id2("Latin-ASCII");
    UnicodeString u_id2(id2.c_str());
    auto transliterator2 = Transliterator::createInstance(u_id2, UTRANS_FORWARD, ec);

    if(U_FAILURE(ec)) {
        throw runtime_error("could not create a transliterator instance: " + id2);
    }

    UnicodeString replacable(x.c_str());
    transliterator1->transliterate(replacable);
    transliterator2->transliterate(replacable);
    std::string result;
    replacable.toUTF8String<std::string>(result);

    delete transliterator1;
    delete transliterator2;
    return result;
}

vector<string> possible_transliterator_ids()
{
    UErrorCode ec;
    auto it = Transliterator::getAvailableIDs(ec);
    if(U_FAILURE(ec)) {
        throw runtime_error("could not create a \"AvailableIDs\" iterator");
    }

    int32_t length;
    vector<string> ids;
    while(auto x = it->next(&length, ec))
    {
        ids.push_back(x);
    }

    delete it;
    return ids;
}

}

