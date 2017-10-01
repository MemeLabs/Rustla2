# - Try to find rapidjson
# This will define
# RAPIDJSON_FOUND
# RAPIDJSON_INCLUDE_DIR
# RAPIDJSON_LIBRARIES

set(RAPIDJSON_SEARCH_PATH "${RAPIDJSON_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/lib/rapidjson")

find_path(RAPIDJSON_INCLUDE_DIR
    NAMES rapidjson/rapidjson.h
    PATHS ${RAPIDJSON_SEARCH_PATH}
    PATH_SUFFIXES include)

set(RAPIDJSON_INCLUDE_DIRS ${RAPIDJSON_INCLUDE_DIR})
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(rapidjson DEFAULT_MSG
    RAPIDJSON_INCLUDE_DIR
    RAPIDJSON_INCLUDE_DIRS)
