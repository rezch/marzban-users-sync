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
add_subdirectory(src/api)

#----------------------- Result path ------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

#----------------------- Executable -------------------------------------------
add_executable(
    main main.cpp)

target_link_libraries(main
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
