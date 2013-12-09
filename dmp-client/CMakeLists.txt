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

find_package(Qt5Widgets REQUIRED)

my_qt5_wrap_ui(QtGeneratedHeaders
                          dmp_client_gui.ui
)

qt5_wrap_cpp(QtGeneratedSources
                          dmp_client_gui.hpp
                          search_result_model.hpp
)

add_executable(dmp-client main.cpp
                          dmp_client.hpp dmp_client.cpp
                          dmp_sender.hpp dmp_sender.cpp
                          dmp_receiver.hpp dmp_receiver.cpp
                          dmp_client_gui.hpp dmp_client_gui.cpp
                          search_result_model.hpp search_result_model.cpp
                          ui_interface.hpp
                          dmp_client_interface.hpp
                          ${QtGeneratedHeaders} ${QtGeneratedSources}
)

if(MINGW)
	set(Boost_USE_STATIC_LIBS ON)
	set(Boost_USE_STATIC_RUNTIME ON)
endif(MINGW)

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

set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
target_link_libraries(dmp-client ${Qt5Widgets_LIBRARIES})

qt5_use_modules(dmp-client Widgets)

if(MINGW)
	add_definitions("-DBOOST_THREAD_USE_LIB=1 -mms-bitfields -DPCRE_STATIC")
	target_link_libraries(dmp-client pthread ws2_32 mswsock ffi)
endif(MINGW)
