cmake_minimum_required(VERSION 3.11)

add_executable(test_reader test_reader.cpp)

target_link_libraries(test_reader PRIVATE
    gtest_main
    MapReduce
)