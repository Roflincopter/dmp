CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

add_definitions("--std=c++11")

if(MINGW)
	cmake_policy(SET CMP0020 NEW)
	set(CMAKE_PREFIX_PATH "${WINSDK_LIB};${QT5PREFIX}")
	set(Boost_USE_STATIC_LIBS ON)
	set(Boost_USE_STATIC_RUNTIME ON)
endif(MINGW)

my_qt5_wrap_ui(QtGeneratedHeaders
	dmp_client_gui.ui
	dmp_client_connect_dialog.ui
	dmp_client_login_dialog.ui
	dmp_client_register_dialog.ui
	dmp_client_error_dialog.ui
	library_dialog.ui
)

qt5_wrap_cpp(QtGeneratedSources
	dmp_client_gui.hpp
	dmp_client_connect_dialog.hpp
	dmp_client_login_dialog.hpp
	dmp_client_register_dialog.hpp
	dmp_client_error_dialog.hpp
	dmp_client_gui_search_bar.hpp
	dmp_client_gui_menu_bar.hpp
	dmp_client_gui_search_results.hpp
	dmp_client_gui_radio_list.hpp
	dmp_client_gui_playlists.hpp
	gui_item_delegate.hpp
	library_dialog.hpp
)

foreach(moc ${QtGeneratedSources})
	set_source_files_properties(${moc} PROPERTIES COMPILE_FLAGS -Wno-undefined-reinterpret-cast)
endforeach(moc)

set(DmpClientSources
	main.cpp
	search_result_model_qt_adapter.hpp search_result_model_qt_adapter.cpp
	search_result_sort_proxy_model.hpp search_result_sort_proxy_model.cpp
	search_bar_model_qt_adapter.hpp search_bar_model_qt_adapter.cpp
	radio_list_model_qt_adapter.hpp radio_list_model_qt_adapter.cpp
	playlists_model_qt_adapter.hpp playlists_model_qt_adapter.cpp
	dmp_client_gui.hpp dmp_client_gui.cpp
	dmp_client_connect_dialog.hpp dmp_client_connect_dialog.cpp
	dmp_client_login_dialog.hpp dmp_client_login_dialog.cpp
	dmp_client_register_dialog.hpp dmp_client_register_dialog.cpp
	dmp_client_error_dialog.hpp dmp_client_error_dialog.cpp
	dmp_client_gui_search_bar.hpp dmp_client_gui_search_bar.cpp
	dmp_client_gui_menu_bar.hpp dmp_client_gui_menu_bar.cpp
	dmp_client_gui_search_results.hpp dmp_client_gui_search_results.cpp
	dmp_client_gui_radio_list.hpp dmp_client_gui_radio_list.cpp
	dmp_client_gui_playlists.hpp dmp_client_gui_playlists.cpp
	dmp_qt_meta_types.hpp
	library_dialog.hpp library_dialog.cpp
	call_event.hpp call_event.cpp
	qt_adapter.hpp
	boost_any_qvariant_convert.hpp
	gui_item_delegate.hpp gui_item_delegate.cpp
	${QtGeneratedHeaders} ${QtGeneratedSources}
)

#set(DmpClientResources resources.qrc)

add_executable(qt-client ${DmpClientSources} ${DmpClientResources})

target_include_directories(qt-client SYSTEM
	PRIVATE ${Boost_INCLUDE_DIRS}
)

target_link_libraries(qt-client
	PRIVATE Qt5::Widgets
	PRIVATE Qt5::Core
	PRIVATE Qt5::Gui
	PRIVATE ${Boost_LIBRARIES}
	PRIVATE dmp-client
	PRIVATE dmp-library
	PRIVATE dmp-common
	PRIVATE dmp-net
	PRIVATE dmp-config
	PRIVATE dbg
	PRIVATE serialization
)

if(MINGW)
	target_compile_options(qt-client
		PRIVATE "-mwindows"
	)
endif(MINGW)

if(USE_GPERF)
	target_link_libraries(qt-client
		PRIVATE "-lprofiler"
	)
endif(USE_GPERF)

sane_install(TARGETS qt-client)
