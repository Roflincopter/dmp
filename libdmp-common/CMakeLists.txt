CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

set(dmp-common-sources
								connection.hpp connection.cpp
								accept.hpp accept.cpp
								connect.hpp connect.cpp
								message.hpp
								message_serializer.hpp
								message_callbacks.hpp
)

if(MINGW)
	add_library(dmp-common STATIC ${dmp-common-sources})
else(MINGW)
	add_library(dmp-common SHARED ${dmp-common-sources})
endif(MINGW)

include_directories(../libdmp-library/include)

find_package(Boost COMPONENTS system thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(dmp-common ${Boost_LIBRARIES})
