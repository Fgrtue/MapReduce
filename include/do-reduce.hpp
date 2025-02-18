#pragma once

#include "commons.hpp"
#include "con-queue.hpp"
#include "map-reduce-user.hpp"

class DoReduce {

private:

public:

    DoReduce(int, vector<reduce_queue>, 
             std::function<int(UserReduce::Key)> hash_reduce,
             UserReduce);

};