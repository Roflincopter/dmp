cmake_minimum_required(VERSION 2.8)

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
		set(outfile ${CMAKE_CURRENT_SOURCE_DIR}/gui/ui_${outfile}.hpp)
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
	gui/dmp_client_gui.ui
	gui/dmp_client_connect_dialog.ui
)

qt5_wrap_cpp(QtGeneratedSources
	gui/dmp_client_gui.hpp
	gui/dmp_client_connect_dialog.hpp
	gui/dmp_client_gui_search_bar.hpp
	gui/dmp_client_gui_menu_bar.hpp
	gui/dmp_client_gui_search_results.hpp
	gui/dmp_client_gui_radio_list.hpp
	gui/dmp_client_gui_playlists.hpp
)

set(DmpClientSources
	main.cpp
	dmp_client.hpp dmp_client.cpp
	dmp_sender.hpp dmp_sender.cpp
	dmp_receiver.hpp dmp_receiver.cpp
	search_result_model.hpp search_result_model.cpp
	gui/search_result_model_qt_adapter.hpp gui/search_result_model_qt_adapter.cpp
	search_bar_model.hpp search_bar_model.cpp
	gui/search_bar_model_qt_adapter.hpp gui/search_bar_model_qt_adapter.cpp
	radio_list_model.hpp radio_list_model.cpp
	gui/radio_list_model_qt_adapter.hpp gui/radio_list_model_qt_adapter.cpp
	playlists_model.hpp playlists_model.cpp
	gui/playlists_model_qt_adapter.hpp gui/playlists_model_qt_adapter.cpp
	gui/dmp_client_gui.hpp gui/dmp_client_gui.cpp
	gui/dmp_client_connect_dialog.hpp gui/dmp_client_connect_dialog.cpp
	gui/dmp_client_gui_search_bar.hpp gui/dmp_client_gui_search_bar.cpp
	gui/dmp_client_gui_menu_bar.hpp gui/dmp_client_gui_menu_bar.cpp
	gui/dmp_client_gui_search_results.hpp gui/dmp_client_gui_search_results.cpp
	gui/dmp_client_gui_radio_list.hpp gui/dmp_client_gui_radio_list.cpp
	gui/dmp_client_gui_playlists.hpp gui/dmp_client_gui_playlists.cpp
	dmp_client_ui_controller_interface.hpp
	dmp_client_ui_delegate.hpp
	dmp_client_interface.hpp
	${QtGeneratedHeaders} ${QtGeneratedSources}
)

#set(DmpClientResources resources.qrc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(dmp-client ${DmpClientSources} ${DmpClientResources})

include_directories(${CMAKE_SOURCE_DIR}/libdmp-library/include)
target_link_libraries(dmp-client dmp-library)

include_directories(${CMAKE_SOURCE_DIR}/libdmp-common)
target_link_libraries(dmp-client dmp-common)

find_package(Boost COMPONENTS system serialization program_options REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
target_link_libraries(dmp-client ${Boost_LIBRARIES})

find_package(GStreamer REQUIRED)
include_directories(SYSTEM ${GSTREAMER_INCLUDE_DIRS})
target_link_libraries(dmp-client ${GSTREAMER_LIBRARIES})

find_package(GLIB2 REQUIRED)
include_directories(SYSTEM ${GLIB2_INCLUDE_DIRS})
target_link_libraries(dmp-client ${GLIB2_LIBRARIES})

find_package(GObject REQUIRED)
include_directories(SYSTEM ${GOBJECT_INCLUDE_DIR})
target_link_libraries(dmp-client ${GOBJECT_LIBRARIES})

find_package(Qt5Widgets REQUIRED)
add_definitions(${Qt5Widgets_DEFINITIONS})

find_package(Qt5Gui REQUIRED)
add_definitions(${Qt5Gui_DEFINITIONS})

find_package(Qt5Core REQUIRED)
add_definitions(${Qt5Gui_DEFINITIONS})

set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} ${Qt5Gui_EXECUTABLE_COMPILE_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
if(MINGW)
	set(Qt5Widgets_LIBRARIES ${Qt5Widgets_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Core_LIBRARIES} jpeg png pcre16 /home/dennis/projects/mxe/usr/x86_64-w64-mingw32/qt5/plugins/platforms/libqwindows.a)
endif(MINGW)
target_link_libraries(dmp-client ${Qt5Widgets_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Core_LIBRARIES})

if(USE_GPERF)
	target_link_libraries(dmp-client "-lprofiler")
endif(USE_GPERF)

qt5_use_modules(dmp-client Widgets)

if(MINGW)
	add_definitions("-DBOOST_THREAD_USE_LIB=1 -mms-bitfields -DPCRE_STATIC")
	#target_link_libraries(dmp-client tcpclientsrc)
	target_link_libraries(dmp-client pthread ws2_32 mswsock ffi imm32 opengl32 freetype bz2 /home/dennis/projects/mxe/usr/x86_64-w64-mingw32/lib/gstreamer-1.0/libgstcoreelements.a)
endif(MINGW)
