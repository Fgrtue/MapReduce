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

    void operator() (const Key& key, const vector<Value>& vec_values) {
        for(auto& elem : vec_values) {
            storage[key] += elem;
        }
    }

    vector<pair<int,Value>> get_values() {
        vector<pair<int,Value>> data;
        for(auto [key, value] : storage) {
            data.emplace_back(std::stoi(key),value);
        }
        std::sort(data.begin(), data.end());
        return data;
    }

private:

    std::unordered_map<Key,Value> storage;

};

using UserMap = Map<std::string, int>;
using UserReduce = Reduce<std::string, int>;

using map_queue    = ConQueue<pair<UserMap::Key, UserMap::Value>>;
using reduce_queue = ConQueue<pair<UserReduce::Key, vector<UserReduce::Value>>>;
