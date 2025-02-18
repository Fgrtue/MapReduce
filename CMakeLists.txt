cmake_minimum_required(VERSION 3.11)

set(PROJECT_NAME MapReduce)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -fno-omit-frame-pointer -g -pthread")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
endif()

include(FetchContent)

# add google test
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip)

FetchContent_MakeAvailable(googletest)

add_subdirectory(test)

# add nlohmann/json library
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(foo PRIVATE nlohmann_json::nlohmann_json)

# 9. Define library sources
file(GLOB_RECURSE HEADERS "include/*.hpp")
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

# 11. Specify include directories
target_include_directories(${PROJECT_NAME} PUBLIC include)

# 12. Attach threads library 
# find_package(Threads REQUIRED)
# target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads rt)

add_executable(map-reduce src/main.cpp)

target_link_libraries(map-reduce PRIVATE ${PROJECT_NAME})