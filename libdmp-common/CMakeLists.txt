add_library(dmp-common STATIC
	gstreamer_base.hpp gstreamer_base.cpp
	number_pool.hpp number_pool.cpp
	radio_state.hpp radio_state.cpp
	key_management.hpp key_management.cpp
	playlist.hpp
)

target_include_directories(dmp-common
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(dmp-common
	PUBLIC PkgConfig::GStreamer
	PUBLIC Boost::headers
	PRIVATE Boost::filesystem
	PUBLIC dmp-library
	PRIVATE boost-fusion-utils
	PRIVATE dmp-config
	PRIVATE dbg
)
