# Try to find libvterm
# Once done this will define
#
#  LIBVTERM_FOUND - system has libvterm
#  LIBVTERM_LIBRARIES - Link these to use libvterm

if(LIBVTERM_LIBRARIES)
    set (LIBVTERM_FIND_QUIETLY TRUE)
endif()

FIND_PATH(LIBVTERM_INCLUDE_DIRS vterm.h)
FIND_LIBRARY(LIBVTERM_LIBRARIES NAMES vterm)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LIBVTERM DEFAULT_MSG LIBVTERM_LIBRARIES LIBVTERM_INCLUDE_DIRS)

if (LIBVTERM_FOUND)
    message (STATUS "Found components for libvterm")
    message (STATUS "LIBVTERM_LIBRARIES = ${LIBVTERM_LIBRARIES}")
else (LIBVTERM_FOUND)
    if (LIBVTERM_FIND_REQUIRED)
        message (FATAL_ERROR "Could not find libvterm!")
    endif (LIBVTERM_FIND_REQUIRED)
endif (LIBVTERM_FOUND)

MARK_AS_ADVANCED(LIBVTERM_LIBRARIES LIBVTERM_INCLUDE_DIRS)
