CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

set(dmp-net-sources
	connection.hpp connection.cpp
	accept.hpp accept.cpp
	connect.hpp connect.cpp
	message.hpp
	message_switch.hpp
	message_callbacks.hpp
)

if(MINGW)
	add_library(dmp-net STATIC ${dmp-net-sources})
else(MINGW)
	add_library(dmp-net SHARED ${dmp-net-sources})
endif(MINGW)

find_package(Boost COMPONENTS system REQUIRED)

target_link_libraries(dmp-net ${Boost_LIBRARIES})
target_link_libraries(dmp-net dmp-library)
target_link_libraries(dmp-net dmp-common)

target_include_directories(dmp-net SYSTEM
	PRIVATE ${Boost_INCLUDE_DIRS}
)

target_include_directories(dmp-net
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE ${CMAKE_SOURCE_DIR}/libdmp-library/include
	PRIVATE ${CMAKE_SOURCE_DIR}/libdmp-common
)


if(USE_GPERF)
	target_link_libraries(dmp-net "-lprofiler")
endif(USE_GPERF)
