cmake_minimum_required(VERSION 4.0)
project(SYNC_UUIDS)

set(CMAKE_CXX_STANDARD 23)

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

add_executable(
    main main.cpp
    host_api_manager.cpp
    user.cpp)

target_link_libraries(main
    PRIVATE Boost::asio
    PRIVATE Boost::thread
    PRIVATE cpr::cpr
    PRIVATE cpp_dotenv
    PRIVATE nlohmann_json::nlohmann_json)
