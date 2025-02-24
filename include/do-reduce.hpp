#pragma once

#include "commons.hpp"
#include "error.hpp"
#include "con-queue.hpp"
#include "map-reduce-user.hpp"

class DoReduce {

private:

    int                   num_reducers_;
    vector<bool>          vec_map_finished_;
    vector<std::thread>   rdsrs_;
    vector<reduce_queue> *reduce_ques_;

    void reduction_worker(reduce_queue&, UserReduce, bool&);

public:

    DoReduce(int, vector<reduce_queue>*, 
             std::function<int(UserReduce::Key)> hash_reduce,
             UserReduce);

    ~DoReduce();
};