cmake_minimum_required(VERSION 3.11)

set(PROJECT_NAME MapReduce)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(${PROJECT_NAME} LANGUAGES CXX)

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
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_MakeAvailable(googletest)

add_subdirectory(test)

# add nlohmann/json library
add_subdirectory(nlohmann_json)

# 9. Define library header
set(HEADERS 
include/commons.hpp
include/do-map.hpp
include/error.hpp
include/reader.hpp
include/con-queue.hpp
include/do-reduce.hpp
include/map-reduce-user.hpp
)

# 10. Define sources
set(SOURCES 
src/do-map.cpp  
src/do-reduce.cpp
src/error.cpp
src/reader.cpp
src/timer.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json)

# 11. Specify include directories
target_include_directories(${PROJECT_NAME} PUBLIC include)

add_executable(map-reduce src/main.cpp)

target_include_directories(map-reduce PUBLIC include)

target_link_libraries(map-reduce PRIVATE ${PROJECT_NAME})

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)