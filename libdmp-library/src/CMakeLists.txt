include_directories("../include")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

file(GLOB dmp-library-sources *.cpp)
file(GLOB dmp-library-headers ../include/*.hpp)

if(MINGW)
	add_library(dmp-library STATIC ${dmp-library-sources} ${dmp-library-headers})
else(MINGW)
	add_library(dmp-library SHARED ${dmp-library-sources} ${dmp-library-headers})
endif(MINGW)

find_package(Boost COMPONENTS system filesystem regex serialization REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(dmp-library ${Boost_LIBRARIES})

find_package(TagLib REQUIRED)

if(MINGW)
	target_link_libraries(dmp-library ${TAGLIB_LIBRARIES} z icudt51)
	add_definitions("-DTAGLIB_STATIC")
else(MINGW)
	target_link_libraries(dmp-library ${TAGLIB_LIBRARIES})
endif(MINGW)

find_package(ICU REQUIRED)

set(BUNDLE_TYPE "static" CACHE STRING
  "ICU resource bundle type; possible values are: common, static and common")
string (TOUPPER ${BUNDLE_TYPE} _BUNDLE_TYPE)

include_directories(ICU_INCLUDE_DIRS)

if (NOT ${_BUNDLE_TYPE} STREQUAL "SHARED")
  add_definitions (-DUSE_STATIC_RESOURCES)
endif (NOT ${_BUNDLE_TYPE} STREQUAL "SHARED")

if (${_BUNDLE_TYPE} STREQUAL "COMMON")
  add_definitions (-DUSE_COMMON_RESOURCES)
endif (${_BUNDLE_TYPE} STREQUAL "COMMON")

#create_icu_resource_bundle (TARGET dmp-library NAME icu_example_data
#  FILES ${PROJECT_RES} ${_BUNDLE_TYPE} RESOURCE_LOCATION _LOCATION
#  CXX_FLAGS _CXX_FLAGS)

target_link_libraries(dmp-library ${ICU_LIBRARIES})
