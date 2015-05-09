CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

set(dmp-net-sources
	connection.hpp connection.cpp
	accept.hpp accept.cpp
	connect.hpp connect.cpp
	message.hpp message.cpp
	message_switch.hpp
	message_callbacks.hpp
)

add_library(dmp-net STATIC ${dmp-net-sources})

find_package(Boost COMPONENTS system REQUIRED)
find_package(Sodium)

set(boost_variant_workaround BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS;BOOST_MPL_LIMIT_LIST_SIZE=30)

if(Boost_MAJOR_VERSION MATCHES 1 AND Boost_MINOR_VERSION MATCHES 58)
	target_compile_definitions(dmp-net PUBLIC "BOOST_PP_VARIADICS=0")
endif()

target_compile_definitions(dmp-net PUBLIC "${boost_variant_workaround}")

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
