
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Core REQUIRED)

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