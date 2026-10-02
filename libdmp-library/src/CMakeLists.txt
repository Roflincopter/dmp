add_library(dmp-library STATIC
	icu-ascii-transliterator.cpp
	library-entry.cpp
	library-search.cpp
	library.cpp
	query-parser.cpp
	query.cpp
	../include/icu-ascii-transliterator.hpp
	../include/library-entry.hpp
	../include/library-search.hpp
	../include/library.hpp
	../include/load_info.hpp
	../include/query-parser.hpp
	../include/query.hpp
)

target_include_directories(dmp-library
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(dmp-library
	PUBLIC boost-fusion-utils
	PUBLIC dmp-config
	PUBLIC Boost::headers
	PUBLIC Boost::filesystem
	PUBLIC Boost::regex
	PUBLIC Boost::serialization
	PRIVATE serialization
	PRIVATE PkgConfig::TagLib
	PRIVATE ICU::uc
	PRIVATE ICU::i18n
	PRIVATE ICU::data
)
