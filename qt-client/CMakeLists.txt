set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_executable(qt-client
	main.cpp
	search_result_model_qt_adapter.hpp search_result_model_qt_adapter.cpp
	search_result_sort_proxy_model.hpp search_result_sort_proxy_model.cpp
	search_bar_model_qt_adapter.hpp search_bar_model_qt_adapter.cpp
	radio_list_model_qt_adapter.hpp radio_list_model_qt_adapter.cpp
	playlists_model_qt_adapter.hpp playlists_model_qt_adapter.cpp
	dmp_client_gui.hpp dmp_client_gui.cpp dmp_client_gui.ui
	dmp_client_connect_dialog.hpp dmp_client_connect_dialog.cpp dmp_client_connect_dialog.ui
	dmp_client_login_dialog.hpp dmp_client_login_dialog.cpp dmp_client_login_dialog.ui
	dmp_client_register_dialog.hpp dmp_client_register_dialog.cpp dmp_client_register_dialog.ui
	dmp_client_error_dialog.hpp dmp_client_error_dialog.cpp dmp_client_error_dialog.ui
	dmp_client_gui_search_bar.hpp dmp_client_gui_search_bar.cpp
	dmp_client_gui_menu_bar.hpp dmp_client_gui_menu_bar.cpp
	dmp_client_gui_search_results.hpp dmp_client_gui_search_results.cpp
	dmp_client_gui_radio_list.hpp dmp_client_gui_radio_list.cpp
	dmp_client_gui_playlists.hpp dmp_client_gui_playlists.cpp
	dmp_qt_meta_types.hpp
	library_dialog.hpp library_dialog.cpp library_dialog.ui
	call_event.hpp call_event.cpp
	qt_adapter.hpp
	boost_any_qvariant_convert.hpp
	gui_item_delegate.hpp gui_item_delegate.cpp
)

target_include_directories(qt-client
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

if(WIN32)
	set_target_properties(qt-client PROPERTIES WIN32_EXECUTABLE ON)
endif()

target_link_libraries(qt-client
	PRIVATE Qt::Widgets
	PRIVATE Qt::Gui
	PRIVATE Qt::Core
	PRIVATE Boost::headers
	PRIVATE Boost::program_options
	PRIVATE Boost::serialization
	PRIVATE dmp-client
	PRIVATE dmp-library
	PRIVATE dmp-common
	PRIVATE dmp-net
	PRIVATE dmp-config
	PRIVATE dbg
	PRIVATE serialization
)

install(TARGETS qt-client RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
