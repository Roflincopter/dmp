cmake_minimum_required(VERSION 2.8.12)

project(dmp)

set(CPACK_PACKAGE_NAME dmp)
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_VERSION 0.1)
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "gstreamer1.0-plugins-bad (>= 1.2), gstreamer1.0-plugins-good (>= 1.2), gstreamer1.0-plugins-base (>= 1.2), libgstreamer1.0-0 (>= 1.2), libtagc0 (>= 1.9), qt5-default (>= 5.2), libboost-all-dev (>= 1.54), libsodium13 (>= 1.0.0)")
set(CPACK_PACKAGE_CONTACT "dennis@brentj.es")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A distributed media player")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

include(CPack)

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

add_definitions(${warnings} -g3 -Wno-shadow)

add_subdirectory(libdbg)

add_subdirectory(libdmp-library)
add_subdirectory(libboost-fusion-utils)
add_subdirectory(libdmp-common)
add_subdirectory(libdmp-net)

add_subdirectory(libdmp-config)

add_subdirectory(dmp-server)
add_subdirectory(dmp-client)
add_subdirectory(qt-client)
