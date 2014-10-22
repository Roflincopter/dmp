# - Try to find LibDMP-library
# Once done this wil define
# ODBc++_FOUND
# ODBc++_INCLUDE_DIRS
# ODBc++_LIBRARIES

find_path(ODBc++_INCLUDE_DIR odb/core.hxx)
find_library(ODBc++_LIBRARY NAMES libodb odb)
find_program(ODBc++_COMPILER NAMES odb)

set(ODBc++_LIBRARIES ${ODBc++_LIBRARY})
set(ODBc++_INCLUDE_DIRS ${ODBc++_INCLUDE_DIR})

foreach(_ODBc++_component ${ODBc++_FIND_COMPONENTS})
	find_path(ODBc++_${_ODBc++_component}_INCLUDE_DIR NAMES "odb/${_ODBc++_component}/sqlite-types.hxx")
	
	find_library(ODBc++_${_ODBc++_component}_LIBRARY NAMES libodb-${_ODBc++_component} odb-${_ODBc++_component})
	
	set(ODBc++_INCLUDE_DIRS ${ODBc++_INCLUDE_DIRS} ${ODBc++_${_ODBc++_component}_INCLUDE_DIR})
	set(ODBc++_LIBRARIES ${ODBc++_LIBRARIES} ${ODBc++_${_ODBc++_component}_LIBRARY})
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ODBc++ REQUIRED_VARS ODBc++_LIBRARY ODBc++_INCLUDE_DIR ODBc++_COMPILER)

set(ODBc++_FOUND ${ODBC++_FOUND})

if(NOT ODBc++_FOUND)
	IF (ODBc++_FIND_REQUIRED)
		message(SEND_ERROR "Unable to find all ODBc++-components or library")
	ENDIF(ODBc++_FIND_REQUIRED)
endif(NOT ODBc++_FOUND)

function(ODB_compile outfiles)
	set(options)
	set(oneValueArgs)
	set(multiValueArgs)

	cmake_parse_arguments(_ODB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	set(ODB_files ${_ODB_UNPARSED_ARGUMENTS})

	set(_our_args
		"--std" "c++11"
		"-DODB_COMPILER"
		"--generate-query"
		"--generate-schema"
		"--default-pointer" "std::shared_ptr"
		"-d" "sqlite"
		"--output-dir" "${CMAKE_CURRENT_SOURCE_DIR}"
		"--hxx-suffix" ".hpp"
		"--cxx-suffix" ".cpp"
		"--ixx-suffix" ".ipp"
	)

	foreach(file ${ODB_files})
		get_filename_component(_infile ${file} ABSOLUTE)
		set(_our_args ${_our_args} "${_infile}")
	endforeach()

	foreach(it ${ODB_files})
		get_filename_component(_infile ${it} ABSOLUTE)
		get_filename_component(_outfile ${it} NAME_WE)

		set(_outfile1 ${_outfile}-odb.hpp)
		add_custom_command(
			OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${_outfile1}
			COMMAND ${ODBc++_COMPILER}
			ARGS ${_our_args}
			DEPENDS ${_infile}
			VERBATIM
		)

		set(_outfile2 ${_outfile}-odb.cpp)
		add_custom_command(
			OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${_outfile2}
			COMMAND ${ODBc++_COMPILER}
			ARGS ${_our_args}
			DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_outfile1}
			VERBATIM
		)

		set(_outfile3 ${_outfile}-odb.ipp)
		add_custom_command(
			OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${_outfile3}
			COMMAND ${ODBc++_COMPILER}
			ARGS ${_our_args}
			DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_outfile2}
			VERBATIM
		)

		#note this file is not generated when using sqlite. So when making the database variable check this.
		#set(_outfile4 ${_outfile}.sql)
		#add_custom_command(
		#	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${_outfile4}
		#	COMMAND ${ODBc++_COMPILER}
		#	ARGS ${_our_args}
		#	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_outfile3}
		#	VERBATIM
		#)

		list(APPEND ${outfiles} ${_outfile1} ${_outfile2} ${_outfile3} ${_outfile4})
	endforeach()
	
	set(${outfiles} ${${outfiles}} PARENT_SCOPE)
endfunction()

mark_as_advanced(ODBc++_INCLUDE_DIR ODBc++_LIBRARY)
