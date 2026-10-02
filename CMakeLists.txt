cmake_minimum_required(VERSION 3.20)

project(dmp LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

include(GNUInstallDirs)
include(DmpWarnings)

option(USE_GPERF "Link against Google performance tools (libprofiler)." OFF)
option(BUILD_SERVER "Build the dmp server." ON)
option(BUILD_QT_CLIENT "Build the Qt client." ON)
option(BUILD_TESTING "Build the tests." ON)

if(USE_GPERF)
	find_library(GPERF_PROFILER_LIBRARY NAMES profiler REQUIRED)
	link_libraries(${GPERF_PROFILER_LIBRARY})
endif()

if(BUILD_SERVER OR BUILD_QT_CLIENT)
	include(FindCommonLibraries)

	add_subdirectory(libdbg)
	add_subdirectory(libserialization)

	add_subdirectory(libdmp-library)
	add_subdirectory(libboost-fusion-utils)
	add_subdirectory(libdmp-common)
	add_subdirectory(libdmp-net)

	add_subdirectory(libdmp-config)

	if(BUILD_TESTING)
		enable_testing()
		add_subdirectory(tests)
	endif()
endif()

if(BUILD_SERVER)
	include(FindServerLibraries)

	add_subdirectory(dmp-server)
endif()

if(BUILD_QT_CLIENT)
	include(FindClientLibraries)

	add_subdirectory(dmp-client)
	add_subdirectory(qt-client)
endif()
