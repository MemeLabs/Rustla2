# - Find uwebsockets
# Find the native uwebsockets includes and libraries
#
#  UWS_INCLUDE_DIR - where to find uws.h, etc.
#  UWS_LIBRARIES   - List of libraries when using uwebsockets.
#  UWS_FOUND       - True if uwebsockets found.

# /usr/lib64/libuWS.so
# /usr/include/uWS/uWS.h

if(UWS_INCLUDE_DIR)
    # Already in cache, be silent
    set(UWS_FIND_QUIETLY TRUE)
endif(UWS_INCLUDE_DIR)

find_path(UWS_INCLUDE_DIR uWS/uWS.h)

find_library(UWS_LIBRARY NAMES libuWS uWS
        PATHS /usr/lib
        /usr/lib64
        /usr/local/lib
        /usr/local/lib64)

# Handle the QUIETLY and REQUIRED arguments and set UWS_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(UWS DEFAULT_MSG UWS_LIBRARY UWS_INCLUDE_DIR)

if(UWS_FOUND)
    set(UWS_LIBRARIES ${UWS_LIBRARY})
else(UWS_FOUND)
    set(UWS_LIBRARIES)
endif(UWS_FOUND)

mark_as_advanced(UWS_INCLUDE_DIR UWS_LIBRARY)
