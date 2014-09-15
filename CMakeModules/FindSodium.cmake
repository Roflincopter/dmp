# - Try to find LibDMP-library
# Once done this wil define
# Sodium_FOUND
# Sodium_INCLUDE_DIRS
# Sodium_LIBRARIES

find_path(Sodium_INCLUDE_DIR sodium.h)
find_library(Sodium_LIBRARY NAMES sodium libsodium)

set(Sodium_LIBRARIES ${Sodium_LIBRARY})
set(Sodium_INCLUDE_DIRS ${Sodium_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sodium REQUIRED_VARS Sodium_LIBRARY Sodium_INCLUDE_DIR)

set(Sodium_FOUND ${ODBC++_FOUND})

if(NOT Sodium_FOUND)
	IF (Sodium_FIND_REQUIRED)
		message(SEND_ERROR "Unable to find Sodium cryptop library")
	ENDIF(Sodium_FIND_REQUIRED)
endif(NOT Sodium_FOUND)

mark_as_advanced(Sodium_INCLUDE_DIR Sodium_LIBRARY)
