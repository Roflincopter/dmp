cmake_minimum_required(VERSION 2.8)

set(DmpClientSources
	dmp_client.hpp dmp_client.cpp
	dmp_sender.hpp dmp_sender.cpp
	dmp_receiver.hpp dmp_receiver.cpp
	search_result_model.hpp search_result_model.cpp
	search_bar_model.hpp search_bar_model.cpp
	radio_list_model.hpp radio_list_model.cpp
	playlists_model.hpp playlists_model.cpp
	dmp_client_ui_controller_interface.hpp
	dmp_client_ui_delegate.hpp
	dmp_client_interface.hpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(dmp-client ${DmpClientSources} ${DmpClientResources})

include_directories(${CMAKE_SOURCE_DIR}/libdmp-library/include)
target_link_libraries(dmp-client dmp-library)

include_directories(${CMAKE_SOURCE_DIR}/libdmp-common)
target_link_libraries(dmp-client dmp-common)

include_directories(${CMAKE_SOURCE_DIR}/libdmp-net)
target_link_libraries(dmp-client dmp-net)

find_package(Boost COMPONENTS system serialization program_options REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
target_link_libraries(dmp-client ${Boost_LIBRARIES})

find_package(GStreamer REQUIRED)
include_directories(SYSTEM ${GSTREAMER_INCLUDE_DIRS})
target_link_libraries(dmp-client ${GSTREAMER_LIBRARIES})
include_directories(SYSTEM ${GSTREAMER_BASE_INCLUDE_DIRS})
target_link_libraries(dmp-client ${GSTREAMER_BASE_LIBRARIES})

find_package(GLIB2 REQUIRED)
include_directories(SYSTEM ${GLIB2_INCLUDE_DIRS})
target_link_libraries(dmp-client ${GLIB2_LIBRARIES})

find_package(GObject REQUIRED)
include_directories(SYSTEM ${GOBJECT_INCLUDE_DIR})
target_link_libraries(dmp-client ${GOBJECT_LIBRARIES})
