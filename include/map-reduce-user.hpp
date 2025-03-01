#pragma once

#include "commons.hpp"

using std::string;
using std::vector;
using std::pair;

/*
    TEST1 COUNTING WORDS
    
    template<class K, class V>
class Map {

public:
    using Key   = K;
    using Value = V;

    vector<pair<Key, Value>> operator() (const string& key, const string& value) {
        vector<pair<Key, Value>> ret;
        string word;
        for(size_t i=0;i<value.size();++i) {
            if (!std::isspace(value[i]) && !std::ispunct(value[i]) ) {
                word.push_back(std::tolower(value[i]));
            } else {
                ret.push_back({word, 1});
                word.clear();
            }
        }
        if (!word.empty()) ret.push_back({word, 1});

        return ret;
    }

};

template<class K, class V>
class Reduce {

public:
    using Key   = K;
    using Value = V;

    pair<Key,Value> operator() (const Key& key, const vector<Value>& vec_values) {
        int res = 0;
        for(auto& elem : vec_values) {
            res += elem;
        }
        return {key, res};
    }

};

using UserMap = Map<std::string, int>;
using UserReduce = Reduce<std::string, int>;

*/

/*

    TEST2 SORTING

template<class K, class V>
class Map {

public:
    using Key   = K;
    using Value = V;

    vector<pair<Key, Value>> operator() (const string& key, const string& value) {
        vector<pair<Key, Value>> ret;
        std::stringstream s(value);
        int num;
        while(s >> num) {
            ret.push_back({num, 0});
        }
        return ret;
    }

};

template<class K, class V>
class Reduce {

public:
    using Key   = K;
    using Value = V;

    pair<Key,Value> operator() (const Key& key, const vector<Value>& vec_values) {
        return {key, 0};
    }

};

using UserMap = Map<int, int>;
using UserReduce = Reduce<int, int>;

// Observe: parameters of map_queue cannot be changes, since we assume that
// we work with strings as values 
using map_queue    = ConQueue<pair<std::string, std::string>>;
using vred_val     = vector<UserReduce::Value>;
using reduce_queue = ConQueue<pair<UserReduce::Key, vred_val>>;

*/