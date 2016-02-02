cmake_minimum_required(VERSION 2.8.12)

project(dmp)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")

include(wubwubcmake/warning_settings.cmake)
get_sane_warning_flags(warnings)

include(wubwubcmake/sane_install.cmake)

if(MINGW)
    include(wubwubcmake/enable_gnu11.cmake)
else()
    include(wubwubcmake/enable_cpp11.cmake)
endif()

include(wubwubcmake/sanitizers.cmake)

include(wubwubcmake/join.cmake)
join("${warnings}" " " warnings)

IF(UNIX AND NOT APPLE)
    option(USE_GPERF "Enable Google performance tools." ON)
ENDIF(UNIX AND NOT APPLE)

option(BUILD_SERVER "Build the dmp server." ON)
option(BUILD_QT_CLIENT "Build the Qt client." ON)

add_definitions(${warnings} -g3 -Wno-shadow)

if(BUILD_SERVER OR BUILD_QT_CLIENT)
	include(FindCommonLibraries)

	add_subdirectory(libdbg)
	add_subdirectory(libserialization)

	add_subdirectory(libdmp-library)
	add_subdirectory(libboost-fusion-utils)
	add_subdirectory(libdmp-common)
	add_subdirectory(libdmp-net)

	add_subdirectory(libdmp-config)
	
	enable_testing()
	add_subdirectory(tests)
endif(BUILD_SERVER OR BUILD_QT_CLIENT)

if(BUILD_SERVER)
	include(FindServerLibraries)
	
	add_subdirectory(dmp-server)
endif(BUILD_SERVER)

if(BUILD_QT_CLIENT)
	include(FindClientLibraries)
	
	add_subdirectory(dmp-client)
	add_subdirectory(qt-client)
endif(BUILD_QT_CLIENT)