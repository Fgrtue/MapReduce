#include <do-map.hpp>
#include <gtest/gtest.h>
#include <ctime>

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

    !!!WARNING!!!

    Use the following Map function in map-reduce-user.hpp for correct tests:

    template<class K, class V>
    class Map {

    public:
        using Key   = K;
        using Value = V;

        vector<pair<Key, Value>> operator() (const string& key, const string& value) {
            vector<pair<Key, Value>> ret;
            std::stringstream data(value);
            int num;
            while(data >> num) {
                ret.emplace_back(key, num);
            }
            return ret;
        }
    };
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
        keys[key] = std::to_string(key);
    }
    auto hash_reducer = [](const UserReduce::Key& key) {
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

TEST(DoMap, LittleContention) {

    int num_map = 2;
    int num_reduce = 2;
    int num_keys = 20;
    int num_values = 512;
    queue<pair<string, string>>             queue_jobs;
    std::shared_ptr<vector<reduce_queue>>   reduce_queues(std::make_shared<vector<reduce_queue>>(num_reduce));
    std::shared_ptr<vector<map_queue>>      map_queues(std::make_shared<vector<map_queue>>(num_map));

    vector<string> keys(num_keys);
    for(int key=0;key<num_keys;++key) {
        keys[key] = std::to_string(key);
    }

    auto hash_reducer = [num_reduce](const UserReduce::Key& key) {
        int num = std::stoi(key);
        return num % num_reduce;
    };

    // Load the jobs in the queue of jobs:
    // 1. For thread i generate a file with key_i value_i_j s.t.:
    //  key_i = "i" val_i_j = "j", for j=0...511
    // This ensures that we will do send just one time

    string data;
    for(int val = 0; val < num_values; ++val) {
        data += std::to_string(val);
        data += " ";            
    }

    for(int key=0; key<(num_keys / 2); ++key) {
        queue_jobs.push({keys[key*2], data});
    }

    for(int key=0; key<(num_keys / 2); ++key) {
        queue_jobs.push({keys[key*2+1], data});
    }


    {
        DoMap(num_map, num_reduce, queue_jobs, map_queues, reduce_queues, hash_reducer);
    }
    for(int r=0;r<num_reduce;++r) {
        auto& q = (*reduce_queues)[r];
        while(!q.empty()) {
            auto [k, vec_v] = q.pop();
            int even_key_out = std::stoi(k) % num_reduce;
            EXPECT_EQ(r % num_reduce, even_key_out);
            for(int j=0;j<num_values;++j) {
                EXPECT_EQ(j, vec_v[j]);
            }
        }
    }
}

TEST(DoMap, FinishEarlyNoContention) {
    int num_map = 2;
    int num_reduce = 2;
    int num_keys = 20;
    int num_values1 = 512;
    int num_values2 = 1024;

    queue<pair<string, string>>             queue_jobs;
    std::shared_ptr<vector<reduce_queue>>   reduce_queues(std::make_shared<vector<reduce_queue>>(num_reduce));
    std::shared_ptr<vector<map_queue>>      map_queues(std::make_shared<vector<map_queue>>(num_map));

    vector<string> keys(num_keys);
    for(int key=0;key<num_keys;++key) {
        keys[key] = std::to_string(key);
    }
    auto hash_reducer = [num_reduce](const UserReduce::Key& key) {
        int num = std::stoi(key);
        return num % num_reduce;
    };

    // Load the jobs in the queue of jobs:
    // 1. For thread i generate a file with key_i value_i_j s.t.:
    //  key_i = "i" val_i_j = "j", for j=0...511
    // This ensures that we will do send just one time

    for(int key=0; key<num_keys; ++key) {
        string data;
        int num_values = (key%2 == 0 ? num_values1 : num_values2);
        for(int val = 0; val < num_values; ++val) {
            data += std::to_string(val);
            data += " ";            
        }
        queue_jobs.push({keys[key], data});
    }
    
    {
        DoMap(num_map, num_reduce, queue_jobs, map_queues, reduce_queues, hash_reducer);
    }
    for(int r=0;r<num_reduce;++r) {
        auto& q = (*reduce_queues)[r];
        int num_values = (r%2 == 0 ? num_values1 : num_values2);
        while(!q.empty()) {
            auto [k, vec_v] = q.pop();
            int even_key_out = std::stoi(k) % num_reduce;
            EXPECT_EQ(r%num_reduce, even_key_out);
            for(int j=0;j<num_values;++j) {
                EXPECT_EQ(j, vec_v[j]);
            }
        }
    }
}

TEST(DoMap, FinishEarlyWithContention) {
    int num_map = 2;
    int num_reduce = 2;
    int num_keys = 20;
    int num_values1 = 512;
    int num_values2 = 1024;

    queue<pair<string, string>>             queue_jobs;
    std::shared_ptr<vector<reduce_queue>>   reduce_queues(std::make_shared<vector<reduce_queue>>(num_reduce));
    std::shared_ptr<vector<map_queue>>      map_queues(std::make_shared<vector<map_queue>>(num_map));

    vector<string> keys(num_keys);
    for(int key=0;key<num_keys;++key) {
        keys[key] = std::to_string(key);
    }
    auto hash_reducer = [num_reduce](const UserReduce::Key& key) {
        int num = std::stoi(key);
        return num % num_reduce;
    };

    // Load the jobs in the queue of jobs:
    // 1. For thread i generate a file with key_i value_i_j s.t.:
    //  key_i = "i" val_i_j = "j", for j=0...511
    // This ensures that we will do send just one time

    string data1;
    for(int val = 0; val < num_values1; ++val) {
        data1 += std::to_string(val);
        data1 += " ";            
    }

    string data2;
    for(int val = 0; val < num_values2; ++val) {
        data2 += std::to_string(val);
        data2 += " ";            
    }

    for(int key=0; key<(num_keys / 2); ++key) {
        queue_jobs.push({keys[key*2], data1});
    }

    for(int key=0; key<(num_keys / 2); ++key) {
        queue_jobs.push({keys[key*2+1], data2});
    }

    
    {
        DoMap(num_map, num_reduce, queue_jobs, map_queues, reduce_queues, hash_reducer);
    }
    
    for(int r=0;r<num_reduce;++r) {
        auto& q = (*reduce_queues)[r];
        int num_values = (r%2 == 0 ? num_values1 : num_values2);
        while(!q.empty()) {
            auto [k, vec_v] = q.pop();
            int even_key_out = std::stoi(k) % num_reduce;
            EXPECT_EQ(r%num_reduce, even_key_out);
            for(int j=0;j<num_values;++j) {
                EXPECT_EQ(j, vec_v[j]);
            }
        }
    }
}

TEST(DoMap, Random) {
    int num_map = 16;
    int num_reduce = 4;
    int num_keys = 200;
    vector<int> num_values(num_keys);
    std::mt19937 gen(time(nullptr));
    std::uniform_int_distribution<> dist(1e4, 1e5);

    queue<pair<string, string>>             queue_jobs;
    std::shared_ptr<vector<reduce_queue>>   reduce_queues(std::make_shared<vector<reduce_queue>>(num_reduce));
    std::shared_ptr<vector<map_queue>>      map_queues(std::make_shared<vector<map_queue>>(num_map));

    vector<string> keys(num_keys);
    for(int key=0;key<num_keys;++key) {
        keys[key] = std::to_string(key);
        num_values[key] = dist(gen);
    }
    auto hash_reducer = [num_reduce](const UserReduce::Key& key) {
        int num = std::stoi(key);
        return num % num_reduce;
    };

    // Load the jobs in the queue of jobs:
    // 1. For thread i generate a file with key_i value_i_j s.t.:
    //  key_i = "i" val_i_j = "j", for j=0...511
    // This ensures that we will do send just one time

    for(int key=0; key<num_keys; ++key) {
        string data;
        int num_val = num_values[key];
        for(int val = 0; val < num_val; ++val) {
            data += std::to_string(val);
            data += " ";            
        }
        queue_jobs.push({keys[key], data});
    }
    
    {
        DoMap(num_map, num_reduce, queue_jobs, map_queues, reduce_queues, hash_reducer);
    }
    
    for(int r=0;r<num_reduce;++r) {
        auto& q = (*reduce_queues)[r];
        while(!q.empty()) {
            auto [k, vec_v] = q.pop();
            int k_num = std::stoi(k);
            for(int j=0;j<num_values[k_num];++j) {
                EXPECT_EQ(j, vec_v[j]);
            }
        }
    }
}