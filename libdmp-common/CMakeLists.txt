cmake_minimum_required(VERSION 2.8.12)

set(dmp-common-sources
	gstreamer_base.hpp gstreamer_base.cpp
	number_pool.hpp number_pool.cpp
	radio_state.hpp radio_state.cpp
	key_management.hpp key_management.cpp
	playlist.hpp
)

add_library(dmp-common STATIC ${dmp-common-sources})

include_directories(${CMAKE_SOURCE_DIR}/libdmp-library/include/)

target_link_libraries(dmp-common
	PRIVATE ${GSTREAMER_LIBRARIES}
	PRIVATE ${GSTREAMER_BASE_LIBRARIES}
	PRIVATE ${Glib_LIBRARIES}
	PRIVATE ${GOBJECT_LIBRARIES}
	PRIVATE ${Boost_LIBRARIES}
	PRIVATE boost-fusion-utils
	PRIVATE dmp-config
	PRIVATE dbg
)

target_include_directories(dmp-common SYSTEM
	PUBLIC ${GSTREAMER_INCLUDE_DIRS}
	PUBLIC ${GSTREAMER_CONFIG_INCLUDE_DIRS}
	PUBLIC ${Glib_INCLUDE_DIRS}
	PRIVATE ${GOBJECT_INCLUDE_DIR}
	PRIVATE ${Boost_INCLUDE_DIRS}
)

target_include_directories(dmp-common
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

if(USE_GPERF)
	target_link_libraries(dmp-common
		PRIVATE "-lprofiler"
	)
endif(USE_GPERF)
