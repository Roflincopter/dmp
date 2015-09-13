
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")

message(STATUS ${CMAKE_MODULE_PATH})

find_package(Boost COMPONENTS filesystem program_options regex serialization thread system REQUIRED)
find_package(TagLib REQUIRED)
find_package(ICU REQUIRED)

set(BUNDLE_TYPE "static" CACHE STRING
	"ICU resource bundle type; possible values are: common, static and common"
)
string (TOUPPER ${BUNDLE_TYPE} _BUNDLE_TYPE)

if (NOT ${_BUNDLE_TYPE} STREQUAL "SHARED")
	add_definitions (-DUSE_STATIC_RESOURCES)
endif (NOT ${_BUNDLE_TYPE} STREQUAL "SHARED")

if (${_BUNDLE_TYPE} STREQUAL "COMMON")
	add_definitions (-DUSE_COMMON_RESOURCES)
endif (${_BUNDLE_TYPE} STREQUAL "COMMON")

find_package(GStreamer REQUIRED)
find_package(GLIB2 REQUIRED)
find_package(GObject REQUIRED)
find_package(Sodium)