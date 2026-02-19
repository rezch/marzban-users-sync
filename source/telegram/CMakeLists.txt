#----------------------- PROJECT CONFIGURATION --------------------------------
cmake_minimum_required(VERSION 4.0)
project(telegram_manager VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(BOOST_USE_MULTITHREADED ON)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE RELEASE)
else()
    string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)
endif()
message(STATUS "Building ${PROJECT_NAME} in ${CMAKE_BUILD_TYPE} mode")

#----------------------- Include Libraries ------------------------------------
set(THIRD_PARTY_BUILD_FOLDER ${CMAKE_SOURCE_DIR}/build)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED)
find_package(CURL)
include_directories(/usr/include ${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
include_directories(${CURL_INCLUDE_DIRS})
add_definitions(-DHAVE_CURL)
add_subdirectory(../../third_party/tgbot-cpp
    ${THIRD_PARTY_BUILD_FOLDER}/tgbot-cpp)

find_package(Patch)
if(Patch_FOUND)
  execute_process(
    COMMAND ${Patch_EXECUTABLE} -p1 -i ../../patches/cpp-dotenv.patch
  )
endif()
add_subdirectory(../../third_party/cpp-dotenv
    ${THIRD_PARTY_BUILD_FOLDER}/cpp-dotenv)

#----------------------- Result path ------------------------------------------
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

#----------------------- Lib --------------------------------------------------
add_executable(${PROJECT_NAME}
    main.cpp)

target_link_libraries(${PROJECT_NAME}
    PUBLIC cpp_dotenv
    PRIVATE TgBot
    PRIVATE ${CMAKE_THREAD_LIBS_INIT}
    PRIVATE ${OPENSSL_LIBRARIES}
    PRIVATE ${Boost_LIBRARIES}
    PRIVATE ${CURL_LIBRARIES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../utils
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
