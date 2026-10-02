# Find the ODB C++ ORM runtime, its database backends and the odb compiler.
#
# Defines the imported target ODBc++::ODBc++ (runtime plus every requested
# backend component) and the function ODB_compile(<outvar> headers...).

find_package(PkgConfig QUIET)

find_path(ODBc++_INCLUDE_DIR odb/core.hxx)
find_library(ODBc++_LIBRARY NAMES odb libodb)
find_program(ODBc++_COMPILER NAMES odb)

if(ODBc++_COMPILER)
	execute_process(
		COMMAND ${ODBc++_COMPILER} --version
		OUTPUT_VARIABLE _odb_version_output
		ERROR_QUIET
	)
	if(_odb_version_output MATCHES "([0-9]+\\.[0-9]+\\.[0-9]+)")
		set(ODBc++_VERSION ${CMAKE_MATCH_1})
	endif()
endif()

set(_odb_component_vars)
foreach(_component IN LISTS ODBc++_FIND_COMPONENTS)
	find_path(ODBc++_${_component}_INCLUDE_DIR NAMES "odb/${_component}/version.hxx")
	find_library(ODBc++_${_component}_LIBRARY NAMES odb-${_component} libodb-${_component})
	if(ODBc++_${_component}_INCLUDE_DIR AND ODBc++_${_component}_LIBRARY)
		set(ODBc++_${_component}_FOUND TRUE)
	endif()
	list(APPEND _odb_component_vars ODBc++_${_component}_LIBRARY)
	mark_as_advanced(ODBc++_${_component}_INCLUDE_DIR ODBc++_${_component}_LIBRARY)
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ODBc++
	REQUIRED_VARS ODBc++_LIBRARY ODBc++_INCLUDE_DIR ODBc++_COMPILER ${_odb_component_vars}
	VERSION_VAR ODBc++_VERSION
	HANDLE_COMPONENTS
)

if(ODBc++_FOUND AND NOT TARGET ODBc++::ODBc++)
	add_library(ODBc++::ODBc++ INTERFACE IMPORTED)
	set(_odb_libs ${ODBc++_LIBRARY})
	set(_odb_includes ${ODBc++_INCLUDE_DIR})
	foreach(_component IN LISTS ODBc++_FIND_COMPONENTS)
		list(PREPEND _odb_libs ${ODBc++_${_component}_LIBRARY})
		list(APPEND _odb_includes ${ODBc++_${_component}_INCLUDE_DIR})
	endforeach()
	list(REMOVE_DUPLICATES _odb_includes)
	set_target_properties(ODBc++::ODBc++ PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${_odb_includes}"
		INTERFACE_LINK_LIBRARIES "${_odb_libs}"
	)
endif()

# ODB_compile(<outvar> header...)
#
# Runs the odb compiler on the given persistent class headers. The generated
# <name>-odb.{hpp,cpp,ipp} files are written to the current binary directory
# and their paths are appended to <outvar>.
function(ODB_compile outfiles)
	set(_odb_args
		--std c++17
		-DODB_COMPILER
		--generate-query
		--generate-schema
		--default-pointer std::shared_ptr
		-d sqlite
		--output-dir ${CMAKE_CURRENT_BINARY_DIR}
		--hxx-suffix .hpp
		--cxx-suffix .cpp
		--ixx-suffix .ipp
		-I ${CMAKE_CURRENT_SOURCE_DIR}
	)

	# Make the odb compiler see the same runtime headers we link against, so a
	# non-system ODB install is not shadowed by an older one in /usr/include.
	get_target_property(_odb_runtime_includes ODBc++::ODBc++ INTERFACE_INCLUDE_DIRECTORIES)
	foreach(_dir IN LISTS _odb_runtime_includes)
		list(APPEND _odb_args -I ${_dir})
	endforeach()

	set(_generated)
	foreach(_header IN LISTS ARGN)
		get_filename_component(_infile ${_header} ABSOLUTE)
		get_filename_component(_name ${_header} NAME_WE)

		set(_outputs
			${CMAKE_CURRENT_BINARY_DIR}/${_name}-odb.hpp
			${CMAKE_CURRENT_BINARY_DIR}/${_name}-odb.cpp
			${CMAKE_CURRENT_BINARY_DIR}/${_name}-odb.ipp
		)

		add_custom_command(
			OUTPUT ${_outputs}
			COMMAND ${ODBc++_COMPILER} ${_odb_args} ${_infile}
			DEPENDS ${_infile}
			COMMENT "Running odb on ${_header}"
			VERBATIM
		)

		list(APPEND _generated ${_outputs})
	endforeach()

	set(${outfiles} ${${outfiles}} ${_generated} PARENT_SCOPE)
endfunction()

mark_as_advanced(ODBc++_INCLUDE_DIR ODBc++_LIBRARY ODBc++_COMPILER)
