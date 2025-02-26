#pragma once

#include "commons.hpp"

using std::string;
using std::vector;
using std::pair;

template<class K, class V>
class Map {

public:
    using Key   = K;
    using Value = V;

    vector<pair<Key, Value>> operator() (const string& key, const string& value) {
        vector<pair<Key, Value>> ret;
        std::stringstream data(value);
        string str;
        while(data >> str) {
            if (str.empty()) break;
            ret.emplace_back(key, std::stoi(str));
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
        int res = 0;
        for(auto& elem : vec_values) {
            res += elem;
        }
        return {key, res};
    }

};

using UserMap = Map<std::string, int>;
using UserReduce = Reduce<std::string, int>;

// Observe: parameters of map_queue cannot be changes, since we assume that
// we work with strings as values 
using map_queue    = ConQueue<pair<std::string, std::string>>;
using vred_val     = vector<UserReduce::Value>;
using reduce_queue = ConQueue<pair<UserReduce::Key, vred_val>>;
