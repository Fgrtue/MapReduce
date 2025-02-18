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
        std::cout << key;
        std::cout << value;
    }

};

template<class K, class V>
class Reduce {

public:
    using Key   = K;
    using Value = V;

    pair<Key, Value> operator() (const Key& key, const vector<Value>& vec_values) {
        std::cout << key;
        for(auto elem : vec_values) {
            std::cout << elem;
        }
    }

};

using UserMap = Map<std::string, int>;
using UserReduce = Reduce<std::string, int>;

using map_queue    = ConQueue<pair<UserMap::Key, UserMap::Value>>;
using reduce_queue = ConQueue<pair<UserReduce::Key, UserReduce::Value>>;
