CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

set(dbg-sources
	debug_macros.hpp
	time_util.hpp time_util.cpp
	timed_debug.hpp timed_debug.cpp
)

add_library(dbg STATIC ${dbg-sources})

target_include_directories(dbg
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

if(USE_GPERF)
	target_link_libraries(dbg "-lprofiler")
endif(USE_GPERF)
