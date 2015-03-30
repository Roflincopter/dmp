CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

#Made my "own" QT5_WRAP_UI because the official one outputs the generated file in the binary dir.
#I'm not a big fan of in source-builds and don't want to add my binary dir to my include path...
function(MY_QT5_WRAP_UI outfiles )
	set(options)
	set(oneValueArgs)
	set(multiValueArgs OPTIONS)

	cmake_parse_arguments(_WRAP_UI "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	set(ui_files ${_WRAP_UI_UNPARSED_ARGUMENTS})
	set(ui_options ${_WRAP_UI_OPTIONS})

	foreach(it ${ui_files})
		get_filename_component(outfile ${it} NAME_WE)
		get_filename_component(infile ${it} ABSOLUTE)
		set(outfile ${CMAKE_CURRENT_SOURCE_DIR}/ui_${outfile}.hpp)
		add_custom_command(OUTPUT ${outfile}
			COMMAND ${Qt5Widgets_UIC_EXECUTABLE}
			ARGS ${ui_options} -o ${outfile} ${infile}
			MAIN_DEPENDENCY ${infile}
			VERBATIM)
		list(APPEND ${outfiles} ${outfile})
	endforeach()
	set(${outfiles} ${${outfiles}} PARENT_SCOPE)
endfunction()

add_definitions("--std=c++11")

if(MINGW)
	cmake_policy(SET CMP0020 NEW)
	set(CMAKE_PREFIX_PATH "${WINSDK_LIB};${QT5PREFIX}")
	set(Boost_USE_STATIC_LIBS ON)
	set(Boost_USE_STATIC_RUNTIME ON)
endif(MINGW)

find_package(Qt5Widgets REQUIRED)

my_qt5_wrap_ui(QtGeneratedHeaders
	dmp_client_gui.ui
	dmp_client_connect_dialog.ui
	dmp_client_login_dialog.ui
	dmp_client_register_dialog.ui
	dmp_client_error_dialog.ui
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
)

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
	call_event.hpp call_event.cpp
	qt_adapter.hpp
	boost_any_qvariant_convert.hpp
	gui_item_delegate.hpp gui_item_delegate.cpp
	${QtGeneratedHeaders} ${QtGeneratedSources}
)

#set(DmpClientResources resources.qrc)

add_executable(qt-client ${DmpClientSources} ${DmpClientResources})

find_package(Boost COMPONENTS system program_options REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Core REQUIRED)

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
)

if(USE_GPERF)
	target_link_libraries(qt-client
		PRIVATE "-lprofiler"
	)
endif(USE_GPERF)

sane_install(TARGETS qt-client)
