CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

set(dmp-net-sources
	connection.hpp connection.cpp
	accept.hpp accept.cpp
	connect.hpp connect.cpp
	message.hpp
	message_switch.hpp
	message_callbacks.hpp
)

add_library(dmp-net STATIC ${dmp-net-sources})

find_package(Boost COMPONENTS system REQUIRED)

target_include_directories(dmp-net SYSTEM
	PRIVATE ${Boost_INCLUDE_DIRS}
)

target_include_directories(dmp-net
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE ${CMAKE_SOURCE_DIR}/libdmp-library/include
	PRIVATE ${CMAKE_SOURCE_DIR}/libdmp-common
)

target_link_libraries(dmp-net
	PRIVATE ${Boost_LIBRARIES}
	PRIVATE dmp-library
	PRIVATE boost-fusion-utils
)

if(WIN32)
	target_link_libraries(dmp-net
		PRIVATE wsock32
		PRIVATE ws2_32
	)
endif()

if(USE_GPERF)
	target_link_libraries(dmp-net "-lprofiler")
endif(USE_GPERF)
