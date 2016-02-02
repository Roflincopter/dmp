cmake_minimum_required(VERSION 2.8.12)

set(serialization_sources archive.hpp)

add_library(serialization STATIC ${serialization_sources})

set_target_properties(serialization PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(serialization
	PRIVATE ${Boost_LIBRARIES}
)

target_include_directories(serialization SYSTEM
	PRIVATE ${Boost_INCLUDE_DIRS}
)

target_include_directories(serialization
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

if(USE_GPERF)
	target_link_libraries(boost-fusion-utils 
		PRIVATE "-lprofiler"
	)
endif(USE_GPERF)
