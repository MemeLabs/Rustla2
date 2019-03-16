cmake_minimum_required(VERSION 3.1)
project(rustla2_api)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -g -pthread -D RAPIDJSON_HAS_STDSTRING=1")

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(uWS REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(SQLite REQUIRED)
find_package(rapidjson REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem system iostreams thread)
find_package(Magic REQUIRED)
find_package(JWT REQUIRED)
find_package(Jansson REQUIRED)
find_package(Folly REQUIRED)
find_package(CURL REQUIRED)
find_package(GLog REQUIRED)
find_package(GFlags REQUIRED)
find_package(Wangle REQUIRED)

set(LIB_HEADER
        ${UWS_INCLUDE_DIR}
        ${OPENSSL_INCLUDE_DIR}
        ${ZLIB_INCLUDE_DIRS}
        ${SQLITE_INCLUDE_DIRS}
        ${RAPIDJSON_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${MAGIC_INCLUDE_DIRS}
        ${JWT_INCLUDE_DIRS}
        ${JANSSON_INCLUDE_DIRS}
        ${FOLLY_INCLUDE_DIRS}
        ${CURL_INCLUDE_DIRS}
        ${GLOG_INCLUDE_DIRS}
        ${GFLAGS_INCLUDE_DIRS}
        ${WANGLE_INCLUDE_DIRS})

set(LIB
        ${UWS_LIBRARIES}
        ${OPENSSL_LIBRARIES}
        ${ZLIB_LIBRARIES}
        ${SQLITE_LIBRARIES}
        ${RAPIDJSON_LIBRARIES}
        ${Boost_LIBRARIES}
        ${MAGIC_LIBRARIES}
        ${JWT_LIBRARIES}
        ${JANSSON_LIBRARIES}
        ${FOLLY_LIBRARIES}
        ${CURL_LIBRARIES}
        ${GLOG_LIBRARIES}
        ${GFLAGS_LIBRARIES}
        ${WANGLE_LIBRARIES})

set(SOURCE_FILES
        src/AdminHTTPService.cpp
        src/APIClient.cpp
        src/APIHTTPService.cpp
        src/AngelThumpClient.cpp
        src/AuthHTTPService.cpp
        src/BannedStreams.cpp
        src/Channel.cpp
        src/Config.cpp
        src/Curl.cpp
        src/HTTPRequest.cpp
        src/HTTPResponseWriter.cpp
        src/HTTPService.cpp
        src/IPRanges.cpp
        src/MIMETypes.cpp
        src/MixerClient.cpp
        src/ServicePoller.cpp
        src/Session.cpp
        src/SmashcastClient.cpp
        src/StaticHTTPService.cpp
        src/Status.cpp
        src/Streams.cpp
        src/Strings.cpp
        src/TwitchClient.cpp
        src/Users.cpp
        src/WSService.cpp
        src/YoutubeClient.cpp
        src/main.cpp)
add_executable(rustla2_api ${SOURCE_FILES})

target_include_directories(rustla2_api PRIVATE ${LIB_HEADER})
target_link_libraries(rustla2_api PRIVATE ${LIB})


find_package(GTest REQUIRED)
set(TEST_LIB_HEADER
        ${LIB_HEADER}
        ${GTEST_INCLUDE_DIRS})

set(TEST_LIB
        ${LIB}
        ${GTEST_LIBRARIES})

add_executable(http_router_test
        tests/HTTPRouterTest.cpp
        src/HTTPRequest.cpp
        src/Session.cpp
        src/Config.cpp)
target_include_directories(http_router_test PRIVATE ${TEST_LIB_HEADER})
target_link_libraries(http_router_test PRIVATE ${TEST_LIB})

add_executable(ip_ranges_test
        tests/IPRangesTest.cpp src/IPRanges.cpp)
target_include_directories(ip_ranges_test PRIVATE ${TEST_LIB_HEADER})
target_link_libraries(ip_ranges_test PRIVATE ${TEST_LIB})

add_executable(curl_test
        tests/CurlTest.cpp
        src/Curl.cpp)
target_include_directories(curl_test PRIVATE ${TEST_LIB_HEADER})
target_link_libraries(curl_test PRIVATE ${TEST_LIB})

add_executable(users_test
        tests/UsersTest.cpp
        src/Channel.cpp
        src/Config.cpp
        src/Status.cpp
        src/Strings.cpp
        src/Users.cpp)
target_include_directories(users_test PRIVATE ${TEST_LIB_HEADER})
target_link_libraries(users_test PRIVATE ${TEST_LIB})

enable_testing()
add_test(router http_router_test)
add_test(ip_ranges ip_ranges_test)
add_test(curl curl_test)
