#include "reader.hpp"
#include <unordered_map>
#include <unordered_set>
#include <gtest/gtest.h>

/*
    TEST CASES

    1. Constructor not succesfull
        -> catch an error
    2. Constructor successfull
        -> check that the file is open
    3. parse file : Error wrong json format
    4. parse file : Not a string value
    5. parse file : Successfull pasing 
        -> of values smaller than chunk size
    6. parse file : Successfull parsing
        -> of values larger than chunk size
*/

TEST(Constructor, Fail) {

    std::string file_name = "../test/foo";
    try {
        Reader reader(file_name);
        EXPECT_TRUE(false);
    } catch (const std::runtime_error& e) {
        std::string err_what = e.what();
        std::string err_expect = "0: couldn't open the file";
        EXPECT_EQ(err_what, err_expect);
    }
}

TEST(Constructor, Success) {

    std::string file_name = "../test/reader_test1.json";
    try {
        Reader reader(file_name);
        EXPECT_TRUE(true);
    } catch (const std::runtime_error& e) {
        EXPECT_TRUE(false);
    }
}

TEST(Parser, ErrorFormat) {

    std::string file_name = "../test/reader_test1.json";
    try {
        Reader reader(file_name);
        reader.parse();
        EXPECT_TRUE(false);
    } catch (const std::runtime_error& e) {
        std::string err_what = e.what();
        std::string err_expect = "0: values must be of string type";
        EXPECT_EQ(err_what, err_expect);
    }
}

TEST(Parser, FormatShort) {

    std::string file_name = "../test/reader_test2.json";
    try {
        Reader reader(file_name);
        auto q = reader.parse();
        // key1 : 1 val
        // key2 : 1 val
        std::unordered_map<string, int> map;
        std::unordered_set<string> saved_keys;
        while(!q.empty()) {
            auto p = q.front();
            q.pop();
            map[p.first]++;
            saved_keys.insert(p.first);
            EXPECT_TRUE(p.second.size() < 1024);
        }
        for(auto& elem : saved_keys) {
            EXPECT_EQ(map[elem], 1);
        }
    } catch (const std::runtime_error& e) {
        EXPECT_TRUE(false);
    }
}

TEST(Parser, FormatLong) {

    std::string file_name = "../test/reader_test3.json";
    try {
        Reader reader(file_name);
        auto q = reader.parse();
        // key1 : 1 val
        // key2 : 1 val
        std::unordered_map<string, int> map;
        std::unordered_set<string> saved_keys;
        while(!q.empty()) {
            auto p = q.front();
            q.pop();
            map[p.first]++;
            saved_keys.insert(p.first);
            EXPECT_TRUE(p.second.size() < 1050);
        }
        for(auto& elem : saved_keys) {
            EXPECT_EQ(map[elem], 2);
        }
    } catch (const std::runtime_error& e) {
        EXPECT_TRUE(false);
    }
}