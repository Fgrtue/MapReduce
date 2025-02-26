#include <do-map.hpp>
#include <gtest/gtest.h>

/*

    TEST CASE:

    1. No mapping threads, just create DoMap;
    2. Two mapping threads, two reduce queues 
        -> No contention for the reduce queues
    3. Two mapping threads, two reduce queues: 
        -> With contention for reduce queues
    4. Two mapping threads, two reduce queues: 
        -> one of mappers finishes earlier (we are sure about it) and helps another, 
        -> no contention for the reduce queues
    5. Two mapping threads, two reduse queues:
        -> one of mappers finished earlier and helps another
        -> with contention for the reduce queues
    6. 16 mapping threads, 4 reduce queues:
        -> contention on every of reduce queue
        -> random set of tasks
*/

TEST(DoMap, Constructor) {

    int num_map = 0;
    int num_reduce = 0;
    queue<pair<string, string>>             queue_jobs;
    std::shared_ptr<vector<reduce_queue>>   reduce_queues(std::make_shared<vector<reduce_queue>>(num_reduce));
    std::shared_ptr<vector<map_queue>>      map_queues(std::make_shared<vector<map_queue>>(num_map));

    auto hash_reducer = [num_reduce](const UserReduce::Key& key) {
        static std::hash<UserReduce::Key> hasher;
        return hasher(key) % num_reduce;
    };

    DoMap(num_map, num_reduce, queue_jobs, map_queues, reduce_queues, hash_reducer);
    EXPECT_TRUE(true);
} 

TEST(DoMap, NoContention) {

    int num_map = 2;
    int num_reduce = 2;
    int num_values = 512;
    queue<pair<string, string>>             queue_jobs;
    std::shared_ptr<vector<reduce_queue>>   reduce_queues(std::make_shared<vector<reduce_queue>>(num_reduce));
    std::shared_ptr<vector<map_queue>>      map_queues(std::make_shared<vector<map_queue>>(num_map));

    vector<string> keys(num_map);
    for(int key=0;key<num_reduce;++key) {
        keys.push_back(std::to_string(key));
    }
    auto hash_reducer = [num_reduce](const UserReduce::Key& key) {
        int num = std::stoi(key);
        return num;
    };

    // Load the jobs in the queue of jobs:
    // 1. For thread i generate a file with key_i value_i_j s.t.:
    //  key_i = "i" val_i_j = "j", for j=0...511
    // This ensures that we will do send just one time

    for(int key=0; key<num_reduce; ++key) {
        string data;
        for(int val = 0; val < num_values; ++val) {
            data += std::to_string(val);
            data += " ";            
        }
        queue_jobs.emplace(keys[key], data);
    }
    
    {
        DoMap(num_map, num_reduce, queue_jobs, map_queues, reduce_queues, hash_reducer);
    }
    for(int key=0;key<num_reduce;++key) {
        auto& q = (*reduce_queues)[key];
        while(!q.empty()) {
            auto [k, vec_v] = q.pop();
            EXPECT_EQ(keys[key], k);
            for(int j=0;j<num_values;++j) {
                EXPECT_EQ(j, vec_v[j]);
            }
        }
    }
}