cmake_minimum_required(VERSION 2.8.12)

set(dbg-sources
	debug_macros.hpp
	time_util.hpp time_util.cpp
	timed_debug.hpp timed_debug.cpp
	core_dump.hpp core_dump.cpp
)

find_package(Boost COMPONENTS thread system REQUIRED)

add_library(dbg SHARED ${dbg-sources})

target_include_directories(dbg
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE ${Boost_INCLUDE_DIRS}
)

target_link_libraries(dbg
	PRIVATE ${Boost_LIBRARIES}
)

if(USE_GPERF)
	target_link_libraries(dbg
		PRIVATE "-lprofiler"
	)
endif(USE_GPERF)
