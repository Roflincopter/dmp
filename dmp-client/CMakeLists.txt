add_library(dmp-client STATIC
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
	delegator.hpp
	playlist_ui_delegate.hpp
	radio_list_ui_delegate.hpp
	search_result_ui_delegate.hpp
	search_bar_ui_delegate.hpp
)

target_include_directories(dmp-client
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(dmp-client
	PUBLIC Boost::headers
	PRIVATE Boost::serialization
	PRIVATE dmp-library
	PRIVATE dmp-common
	PRIVATE dmp-net
	PRIVATE dmp-config
	PUBLIC boost-fusion-utils
	PRIVATE dbg
	PRIVATE serialization
)
