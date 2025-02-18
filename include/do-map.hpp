#pragma once

#include "commons.hpp"
#include "con-queue.hpp"
#include "map-reduce-user.hpp"

class DoMap {

private:

public:

    DoMap(int, int, queue<tuple<string,string,string>>&, 
            vector<map_queue>, vector<reduce_queue>, 
            std::function<int(string)>, 
            std::function<int(UserReduce::Key)>,
            UserMap);

};