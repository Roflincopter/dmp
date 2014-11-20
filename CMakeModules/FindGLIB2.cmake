# - Try to find Glib-2.0
# Once done, this will define
#
#  Glib_FOUND - system has Glib
#  Glib_INCLUDE_DIRS - the Glib include directories
#  Glib_LIBRARIES - link these to use Glib

# Use pkg-config to get hints about paths

# Main include dir
find_path(Glib_INCLUDE_DIR
  NAMES glib.h
  PATH_SUFFIXES glib-2.0
)

# Glib-related libraries also use a separate config header, which is in lib dir
find_path(GlibConfig_INCLUDE_DIR
  NAMES glibconfig.h
  PATH_SUFFIXES lib/x86_64-linux-gnu/glib-2.0/include lib/glib-2.0/include ../lib/glib-2.0/include
)

# Finally the library itself
find_library(Glib_LIBRARY
  NAMES glib-2.0
)

set(Glib_INCLUDE_DIRS ${Glib_INCLUDE_DIR} ${GlibConfig_INCLUDE_DIR})
set(Glib_LIBRARIES ${Glib_LIBRARY})

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Glib REQUIRED_VARS Glib_INCLUDE_DIR GlibConfig_INCLUDE_DIR Glib_LIBRARY)
