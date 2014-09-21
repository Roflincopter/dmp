cmake_minimum_required(VERSION 2.8.12)

find_package(Boost COMPONENTS system REQUIRED)
find_package(ODBc++ COMPONENTS sqlite REQUIRED)

ODB_compile(odb_sources
	user.hpp
	radio.hpp
	radio_admin.hpp
)

add_executable(dmp-server
	main.cpp
	dmp_server.hpp dmp_server.cpp
	dmp_radio.hpp dmp_radio.cpp
	dmp_radio_endpoint.hpp dmp_radio_endpoint.cpp
	client_endpoint.hpp client_endpoint.cpp
	dmp_server_interface.hpp
	database.hpp database.cpp
	database_version.hpp
	database_migration.hpp
	user.hpp user.cpp
	radio.hpp radio.cpp
	radio_admin.hpp radio_admin.cpp
	${odb_sources}
)

target_include_directories(dmp-server SYSTEM
	PRIVATE ${Boost_INCLUDE_DIRS}
)

target_include_directories(dmp-server
	PRIVATE ${ODBc++_INCLUDE_DIRECTORIES}
)

target_link_libraries(dmp-server
	PRIVATE ${Boost_LIBRARIES}
	PRIVATE ${ODBc++_LIBRARIES}
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
