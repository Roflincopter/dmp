cmake_minimum_required(VERSION 2.8.12)

set(dmp-config-sources
	dmp_config.hpp dmp_config.cpp
)

add_library(dmp-config STATIC ${dmp-config-sources})

target_link_libraries(dmp-config
	PRIVATE ${Boost_LIBRARIES}
	PRIVATE dbg
	PRIVATE boost-fusion-utils
)

target_include_directories(dmp-config SYSTEM
	PRIVATE ${Boost_INCLUDE_DIRS}
)

target_include_directories(dmp-config
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

if(USE_GPERF)
	target_link_libraries(dmp-config
		PRIVATE "-lprofiler"
	)
endif(USE_GPERF)
