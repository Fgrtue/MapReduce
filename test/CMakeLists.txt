cmake_minimum_required(VERSION 3.11)

add_executable(test_reader test_reader.cpp)

target_link_libraries(test_reader PRIVATE
    gtest_main
    MapReduce
)

add_executable(test_reduce test_reduce.cpp)

target_link_libraries(test_reduce PRIVATE
    gtest_main
    MapReduce
)

add_executable(test_map test_map.cpp)

target_link_libraries(test_map PRIVATE
    gtest_main
    MapReduce
)

add_executable(word_count word_count.cpp)

target_link_libraries(word_count PRIVATE
    gtest_main
    MapReduce
)