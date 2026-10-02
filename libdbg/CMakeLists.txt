add_library(dbg STATIC
	debug_macros.hpp
	time_util.hpp time_util.cpp
	core_dump.hpp core_dump.cpp
)

target_include_directories(dbg
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(dbg
	PRIVATE Boost::headers
)

if(WIN32)
	target_link_libraries(dbg
		PUBLIC ws2_32
	)
endif()
