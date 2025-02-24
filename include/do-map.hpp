#pragma once

#include "commons.hpp"
#include "con-queue.hpp"
#include "map-reduce-user.hpp"

class DoMap {

private:

public:

    DoMap(int, int, queue<pair<string,string>>&, 
            vector<std::unique_ptr<map_queue>>*, 
            vector<std::unique_ptr<reduce_queue>>*, 
            std::function<int(UserReduce::Key)>,
            UserMap);

};