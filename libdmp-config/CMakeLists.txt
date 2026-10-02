add_library(dmp-config STATIC
	dmp_config.hpp dmp_config.cpp
)

target_include_directories(dmp-config
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(dmp-config
	PUBLIC Boost::headers
	PUBLIC Boost::filesystem
	PRIVATE dbg
	PRIVATE boost-fusion-utils
)
