cmake_minimum_required(VERSION 2.8)

set(DmpClientSources
	dmp_client.hpp dmp_client.cpp
	dmp_sender.hpp dmp_sender.cpp
	dmp_receiver.hpp dmp_receiver.cpp
	search_result_model.hpp search_result_model.cpp
	search_bar_model.hpp search_bar_model.cpp
	radio_list_model.hpp radio_list_model.cpp
	playlists_model.hpp playlists_model.cpp
	dmp_client_ui_controller_interface.hpp
	dmp_client_ui_delegate.hpp
	dmp_client_interface.hpp
)

add_library(dmp-client STATIC ${DmpClientSources} ${DmpClientResources})

target_include_directories(dmp-client
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(dmp-client
	dmp-library
	dmp-common
	dmp-net
)
