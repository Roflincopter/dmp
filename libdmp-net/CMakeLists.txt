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
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
target_link_libraries(dmp-net ${Boost_LIBRARIES})

include_directories(${CMAKE_SOURCE_DIR}/libdmp-library/include)
target_link_libraries(dmp-net dmp-library)

include_directories(${CMAKE_SOURCE_DIR}/libdmp-common)
target_link_libraries(dmp-net dmp-common)

if(USE_GPERF)
	target_link_libraries(dmp-net "-lprofiler")
endif(USE_GPERF)
