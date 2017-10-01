# - Try to find JWT
# Once done this will define
#  JWT_FOUND - System has JWT
#  JWT_INCLUDE_DIRS - The JWT include directories
#  JWT_LIBRARIES - The libraries needed to use JWT
#  JWT_DEFINITIONS - Compiler switches required for using JWT

set(JWT_SEARCH_PATH "${JWT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/lib/jwt-cpp")

find_package(PkgConfig)
pkg_check_modules(PC_JWT QUIET jwt)
set(JWT_DEFINITIONS ${PC_JWT_CFLAGS_OTHER})

find_path(JWT_INCLUDE_DIR jwt/jwt_all.h
          HINTS ${PC_JWT_INCLUDEDIR} ${PC_JWT_INCLUDE_DIRS}
          PATH_SUFFIXES jwt)

find_library(JWT_LIBRARY NAMES jwt
             HINTS ${PC_JWT_LIBDIR} ${PC_JWT_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set JWT_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(JWT DEFAULT_MSG
                                  JWT_LIBRARY JWT_INCLUDE_DIR)

mark_as_advanced(JWT_INCLUDE_DIR JWT_LIBRARY)

set(JWT_LIBRARIES ${JWT_LIBRARY})
set(JWT_INCLUDE_DIRS ${JWT_INCLUDE_DIR})
