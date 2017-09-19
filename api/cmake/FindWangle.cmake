#
# Copyright (C) 2015, Yeolar
#
# - Try to find wangle
# This will define
# WANGLE_FOUND
# WANGLE_INCLUDE_DIR
# WANGLE_LIBRARIES

cmake_minimum_required(VERSION 2.8.8)

include(FindPackageHandleStandardArgs)

find_library(WANGLE_LIBRARY wangle PATHS ${WANGLE_LIBRARYDIR})
find_path(WANGLE_INCLUDE_DIR "wangle/acceptor/Acceptor.h" PATHS ${WANGLE_INCLUDEDIR})

set(WANGLE_LIBRARIES ${WANGLE_LIBRARY})

find_package_handle_standard_args(Wangle
    REQUIRED_ARGS WANGLE_INCLUDE_DIR WANGLE_LIBRARIES)
