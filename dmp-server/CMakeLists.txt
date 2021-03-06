cmake_minimum_required(VERSION 2.8.12)

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
	server_exceptions.hpp server_exceptions.cpp
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
	PRIVATE dmp-config
	PRIVATE boost-fusion-utils
	PRIVATE dbg
	PRIVATE serialization
)

if(USE_GPERF)
	target_link_libraries(dmp-server
		PRIVATE "-lprofiler"
	)
endif(USE_GPERF)

sane_install(TARGETS dmp-server)
