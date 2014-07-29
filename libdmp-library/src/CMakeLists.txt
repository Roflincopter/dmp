include_directories("../include")

file(GLOB dmp-library-sources *.cpp)
file(GLOB dmp-library-headers ../include/*.hpp)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

if(MINGW)
	add_library(dmp-library STATIC ${dmp-library-sources} ${dmp-library-headers})
else(MINGW)
	add_library(dmp-library SHARED ${dmp-library-sources} ${dmp-library-headers})
endif(MINGW)

find_package(Boost COMPONENTS system filesystem regex serialization REQUIRED)
find_package(TagLib REQUIRED)
find_package(ICU REQUIRED)

set(BUNDLE_TYPE "static" CACHE STRING
  "ICU resource bundle type; possible values are: common, static and common")
string (TOUPPER ${BUNDLE_TYPE} _BUNDLE_TYPE)

if (NOT ${_BUNDLE_TYPE} STREQUAL "SHARED")
  add_definitions (-DUSE_STATIC_RESOURCES)
endif (NOT ${_BUNDLE_TYPE} STREQUAL "SHARED")

if (${_BUNDLE_TYPE} STREQUAL "COMMON")
  add_definitions (-DUSE_COMMON_RESOURCES)
endif (${_BUNDLE_TYPE} STREQUAL "COMMON")

target_include_directories(dmp-library SYSTEM
	PRIVATE ${Boost_INCLUDE_DIRS}
	PRIVATE ${TAGLIB_INCLUDES}
	PRIVATE ${ICU_INCLUDE_DIRS}
)

target_include_directories(dmp-library
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(dmp-library ${Boost_LIBRARIES})
target_link_libraries(dmp-library ${TAGLIB_LIBRARIES})
target_link_libraries(dmp-library ${ICU_LIBRARIES})

if(MINGW)
	target_link_libraries(dmp-library z icudt51)
	add_definitions("-DTAGLIB_STATIC")
endif(MINGW)

if(USE_GPERF)
    target_link_libraries(dmp-library "-lprofiler")
endif(USE_GPERF)
