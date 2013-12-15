cmake_minimum_required(VERSION 2.8)

project(DMP)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

include(wubwubcmake/warning_settings.cmake)
get_sane_warning_flags(warnings)

if(MINGW)
    include(wubwubcmake/enable_gnu11.cmake)
else()
    include(wubwubcmake/enable_cpp11.cmake)
endif()

include(wubwubcmake/address_sanitizer.cmake)

include(wubwubcmake/join.cmake)
join("${warnings}" " " warnings)

IF(UNIX AND NOT APPLE)
    option(USE_GPERF "Enable Google performance tools." ON)
ENDIF(UNIX AND NOT APPLE)

add_definitions(${warnings} -g3 -Wno-shadow -Wno-switch-enum)

add_subdirectory(libdmp-library)
add_subdirectory(libdmp-common)

add_subdirectory(dmp-server)
add_subdirectory(dmp-client)
