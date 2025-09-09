#----------------------- PROJECT CONFIGURATION --------------------------------
cmake_minimum_required(VERSION 4.0)
project(sync_uuids VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE RELEASE)
else()
    string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)
endif()
message(STATUS "Building SYNC_UUIDS in ${CMAKE_BUILD_TYPE} mode")

#----------------------- Libraries --------------------------------------------
include(FetchContent)

set(BOOST_ENABLE_CMAKE ON)
set(BOOST_INCLUDE_LIBRARIES
    asio
    thread)
FetchContent_Declare(
    Boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_PROGRESS TRUE
    GIT_TAG boost-1.89.0)
FetchContent_MakeAvailable(Boost)

FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.12.0)
FetchContent_MakeAvailable(cpr)

find_package(nlohmann_json 3.12.0 REQUIRED)

add_subdirectory(third_party/cpp-dotenv)

#----------------------- Result path ------------------------------------------
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

#----------------------- Lib --------------------------------------------------
file(GLOB LIB_SOURCES
    src/host_api_manager.cpp
    src/user/user.cpp
)
file(GLOB LIB_HEADERS src/*.h)

add_library(sync_api
    ${LIB_SOURCES} ${LIB_HEADERS})

target_link_libraries(sync_api
    PRIVATE Boost::asio
    PRIVATE Boost::thread
    PRIVATE cpr::cpr
    PRIVATE cpp_dotenv
    PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(sync_api
    PUBLIC include)

#----------------------- Executable -------------------------------------------
add_executable(
    main main.cpp)

target_link_libraries(main
    PRIVATE cpp_dotenv
    PUBLIC sync_api)

target_include_directories(main
    PUBLIC include)

#----------------------- Build ------------------------------------------------
if ("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
    target_compile_options(main PRIVATE
        -g -Wall -O0
    )
else()
    target_compile_options(main PRIVATE
        -O3
    )
endif()
