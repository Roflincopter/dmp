cmake_minimum_required(VERSION 2.8)

add_executable(dmp-server main.cpp
                          dmp_server.hpp dmp_server.cpp
                          dmp_radio.hpp dmp_radio.cpp
                          dmp_radio_endpoint.hpp dmp_radio_endpoint.cpp
                          client_endpoint.hpp client_endpoint.cpp
                          dmp_server_interface.hpp
)

find_package(Boost COMPONENTS system REQUIRED)

target_include_directories(dmp-server SYSTEM
	PRIVATE ${Boost_INCLUDE_DIRS}
)

target_link_libraries(dmp-server
	dmp-library
	dmp-common
	dmp-net
	${Boost_LIBRARIES}
)

if(USE_GPERF)
	target_link_libraries(dmp-server "-lprofiler")
endif(USE_GPERF)

if(MINGW)
	add_definitions("-DBOOST_THREAD_USE_LIB=1 -mms-bitfields -DPCRE_STATIC")
	target_link_libraries(dmp-server pthread ws2_32 mswsock ffi /home/dennis/projects/mxe/usr/x86_64-w64-mingw32.shared/lib/gstreamer-1.0/libgsttcp.dll.a)
endif(MINGW)
