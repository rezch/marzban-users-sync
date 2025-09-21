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
    algorithm
    asio
    thread)
FetchContent_Declare(
    Boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_PROGRESS TRUE
    GIT_TAG boost-1.89.0
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(Boost)

FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.12.0
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(cpr)

find_package(nlohmann_json 3.12.0 REQUIRED)

find_package(Patch)
if(Patch_FOUND)
  execute_process(
    COMMAND ${Patch_EXECUTABLE} -p1 -i ${CMAKE_SOURCE_DIR}/patches/cpp-dotenv.patch
  )
endif()
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/cpp-dotenv build)

#----------------------- Result path ------------------------------------------
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

#----------------------- Lib --------------------------------------------------
file(GLOB LIB_SOURCES
    host_api_manager.cpp
    user/user.cpp
)

file(GLOB LIB_HEADERS src/*.h include/*.h)

add_library(sync_api
    ${LIB_SOURCES} ${LIB_HEADERS})

target_link_libraries(sync_api
    ${Boost_LIBRARIES}
    PRIVATE cpr::cpr
    PUBLIC cpp_dotenv
    PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(sync_api
    PUBLIC include)
