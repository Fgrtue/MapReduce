#pragma once

#include "commons.hpp"
#include "error.hpp"
#include "con-queue.hpp"
#include "map-reduce-user.hpp"

class DoReduce {

private:

    int                                       num_reducers_;
    vector<std::atomic<bool>>                 vec_map_finished_;
    vector<std::thread>                       rdsrs_;
    std::shared_ptr<vector<reduce_queue>>     reduce_ques_;

    void reduction_worker(int, reduce_queue&, UserReduce, std::atomic<bool>&);

public:

    DoReduce(int, const std::shared_ptr<vector<reduce_queue>>&,
             UserReduce);

    ~DoReduce();
};