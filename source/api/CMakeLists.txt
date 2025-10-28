#----------------------- PROJECT CONFIGURATION --------------------------------
cmake_minimum_required(VERSION 4.0)
project(host_api_manager VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE RELEASE)
else()
    string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)
endif()
message(STATUS "Building ${PROJECT_NAME} in ${CMAKE_BUILD_TYPE} mode")

#----------------------- Include Libraries ------------------------------------
include(FetchContent)

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
add_library(${PROJECT_NAME} STATIC)

target_link_libraries(${PROJECT_NAME}
    PRIVATE cpr::cpr
    PUBLIC cpp_dotenv
    PRIVATE nlohmann_json::nlohmann_json)

target_sources(${PROJECT_NAME}
    PRIVATE
        host_api_manager.cpp
        user/user.cpp)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/common
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
