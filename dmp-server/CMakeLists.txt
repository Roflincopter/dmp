cmake_minimum_required(VERSION 2.8.12)

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
	PRIVATE ${Boost_LIBRARIES}
	PRIVATE dmp-library
	PRIVATE dmp-common
	PRIVATE dmp-net
	PRIVATE boost-fusion-utils
	PRIVATE dbg
)

if(USE_GPERF)
	target_link_libraries(dmp-server
		PRIVATE "-lprofiler"
	)
endif(USE_GPERF)

if(MINGW)
	add_definitions("-DBOOST_THREAD_USE_LIB=1 -mms-bitfields -DPCRE_STATIC")
	target_link_libraries(dmp-server
		PRIVATE pthread
		PRIVATE ws2_32
		PRIVATE mswsock
		PRIVATE ffi
	)
endif(MINGW)
