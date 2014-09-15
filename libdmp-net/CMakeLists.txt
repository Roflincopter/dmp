CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

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
find_package(Sodium)

#appearantly Boost 1.56 Variant still uses the MPL library so we still need to increase the MPL list size.
if(NOT (${Boost_MAJOR_VERSION} GREATER 0 AND ${Boost_MINOR_VERSION} GREATER 56))
	set(boost_variant_workaround BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS;BOOST_MPL_LIMIT_LIST_SIZE=30)
	target_compile_definitions(dmp-net PUBLIC "${boost_variant_workaround}")
endif()

target_include_directories(dmp-net SYSTEM
	PRIVATE ${Boost_INCLUDE_DIRS}
)

target_include_directories(dmp-net
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE ${Sodium_INCLUDE_DIRECTORIES}
)

target_link_libraries(dmp-net
	PRIVATE ${Boost_LIBRARIES}
	PRIVATE ${Sodium_LIBRARIES}
	PRIVATE dmp-library
	PRIVATE dmp-common
	PRIVATE boost-fusion-utils
	PUBLIC pthread
	PRIVATE dbg
)

if(WIN32)
	target_link_libraries(dmp-net
		PRIVATE wsock32
		PRIVATE ws2_32
	)
endif()

if(USE_GPERF)
	target_link_libraries(dmp-net
		PRIVATE "-lprofiler"
	)
endif(USE_GPERF)
