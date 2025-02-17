#pragma once

#include "commons.hpp"

template<class K, class V>
class Map {

public:
    using Key   = K;
    using Value = V;

    std::vector<std::pair<Key, Value>> operator() (const std::string& key, const std::string& value) {

    }

};

template<class K, class V>
class Reduce {

public:
    using Key   = K;
    using Value = V;

    std::pair<Key, Value> operator() (const Key& key, const std::vector<Value>& vec_values) {

    }

};

