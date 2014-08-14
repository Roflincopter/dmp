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

#Increasing the maximum number types in a boost::variant.
#Boost 1.56 will introduce boost.variant with variadic templates so this is a workaround untill boost 1.56 comes out.
if(NOT (${Boost_MAJOR_VERSION} GREATER 0 AND ${Boost_MINOR_VERSION} GREATER 55))
	set(boost_variant_workaround BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS;BOOST_MPL_LIMIT_LIST_SIZE=30;BOOST_MPL_LIMIT_VECTOR_SIZE=30)
	target_compile_definitions(dmp-net PUBLIC "${boost_variant_workaround}")
endif()

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
