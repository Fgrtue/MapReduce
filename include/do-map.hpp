#pragma once

#include "commons.hpp"
#include "con-queue.hpp"
#include "map-reduce-user.hpp"

class DoMap {

private:

    int                                   num_map_;
    int                                   num_reducers_;
    std::shared_ptr<vector<map_queue>>    map_queues_;
    std::shared_ptr<vector<reduce_queue>> reduce_queues_;
    std::function<int(UserReduce::Key)>   hash_reduce_;
    vector<int>                           work_to_do_;
    vector<std::thread>                   map_workers_;
    thread_local static UserMap           map_func_;
    thread_local static deque<pair<UserReduce::Key, UserReduce::Value>>      store;
    thread_local static std::unordered_map<UserReduce::Key, vred_val>         key_vvalues;
    thread_local static vector<vector<pair<UserReduce::Key, vred_val>>>       ready_send;

    void Send(deque<pair<UserReduce::Key, UserReduce::Value>>&);

    void map_worker(int);

public:

    DoMap(int, int, queue<pair<string,string>>&, 
            const std::shared_ptr<vector<map_queue>>&, 
            const std::shared_ptr<vector<reduce_queue>>&, 
            std::function<int(UserReduce::Key)>);

    ~DoMap();
};