add_library(dmp-net STATIC
	connection.hpp connection.cpp
	accept.hpp accept.cpp
	connect.hpp connect.cpp
	message.hpp message.cpp
	message_switch.hpp
	message_callbacks.hpp
)

# The message variant has more alternatives than Boost.MPL's preprocessed
# headers support by default.
target_compile_definitions(dmp-net PUBLIC
	BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
	BOOST_MPL_LIMIT_LIST_SIZE=30
)

target_include_directories(dmp-net
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(dmp-net
	PUBLIC Boost::headers
	PUBLIC PkgConfig::Sodium
	PUBLIC Threads::Threads
	PUBLIC boost-fusion-utils
	PUBLIC dmp-library
	PUBLIC dmp-common
	PRIVATE dbg
	PRIVATE serialization
)

if(WIN32)
	target_link_libraries(dmp-net
		PRIVATE ws2_32 mswsock
	)
endif()
