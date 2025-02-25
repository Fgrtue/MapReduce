#include "do-reduce.hpp"
#include <gtest/gtest.h>


/*

    TEST CASES:

    1. Check that you can creade two reducers
        -> first use one upload work thread
        -> let it push the work into the queues of the reducers
        -> and for them to 
            ->write the output of the results in corresponding files
    2. Create two reducers and two threads for uploading work
        -> check that uploaders can seamlessly upload work to reducers
        -> check that reducers can perform their work normally
    3. Do the second case for 16 uploaders and 4 reducers

*/


TEST(Reducer, OneTwo) {

    // 1. Create reduce queues
    // 2. Create reduce_function
    // 3. Use number of threads = 2
    // 4. Create a queue of tasks T
    // 5. put there keys: 1, 2, 3 ... 99
    //      -> and values {1, 1, ... 1} | of length 100
    // 6. Send them both to reducer 1 and reducer 2 using the lock in the queue

    int num_reduce = 2;
    std::shared_ptr<vector<reduce_queue>>    reduce_ques(std::make_shared<vector<reduce_queue>>(num_reduce));
    UserReduce reduce_function;

    int num_keys = 100;
    int num_val = 100;
    {
        DoReduce reduction_process(num_reduce, reduce_ques, reduce_function);    
        vector<vector<int>> data(num_keys);
        for(int key=0;key<num_keys;++key) {
            for(int j=0;j<num_val;++j) {
                data[key].push_back(1);
            }
        }
        for(int key=0;key<num_keys;++key) {
            string str = std::to_string(key);
            for(int t=0; t<num_reduce;++t) {
                reduce_queue&  q = (*reduce_ques)[t];
                std::lock_guard lk_(q.mt_);
                q.push(std::make_pair(str, data[key]));
                q.cv_empty_.notify_one();
            }
        }
    }
    // read the file and compare with desired values
    for (int r=0;r<num_reduce;++r) {
        string file_name = "reduction_" + std::to_string(r);
        std::fstream file(file_name);
        vector<pair<int, int>> storage;
        for(int k=0;k<num_keys;++k) {
            int key;
            int val;
            file >> key >> val;
            storage.emplace_back(key,val);
        }
        std::sort(storage.begin(), storage.end());
        for(int k=0;k<num_keys;++k) {
            int key = storage[k].first;
            int val = storage[k].second;
            EXPECT_EQ(key, k);
            EXPECT_EQ(val, num_val);
        }
    }
}

void fill_data(int num_reduce, int num_keys, int num_val, 
    std::shared_ptr<vector<reduce_queue>> reduce_ques) {
    vector<vector<int>> data(num_keys);
    for(int key=0;key<num_keys;++key) {
        for(int j=0;j<num_val;++j) {
            data[key].push_back(1);
        }
    }
        for(int key=0;key<num_keys;++key) {
            string str = std::to_string(key);
            reduce_queue&  q = (*reduce_ques)[key%num_reduce];
            std::lock_guard lk_(q.mt_);
            q.push(std::make_pair(str, data[key]));
            q.cv_empty_.notify_one();
        }
}

TEST(Reducer, TwoTwo) {

    int num_producers = 2;
    int num_reduce = 2;
    std::shared_ptr<vector<reduce_queue>>    reduce_ques(std::make_shared<vector<reduce_queue>>(num_reduce));
    UserReduce reduce_function;

    int num_keys = 100;
    int num_val = 100;
    {
        DoReduce reduction_process(num_reduce, reduce_ques, reduce_function);    
        // create two threads, each filling the queues with their own keys
        // first producer fills only even keys
        // second producers fills only odd keys
        vector<std::thread> producers;
        for (int i=0;i<num_producers;++i) {
            producers.emplace_back(&fill_data, num_reduce, num_keys, num_val, reduce_ques);
        }
        for(int i=0;i<num_producers;++i) {
            producers[i].join();
        }
    }
    // read the file and compare with desired values
    for (int r=0;r<num_reduce;++r) {
        vector<pair<int, int>> storage;
        string file_name = "reduction_" + std::to_string(r);
        std::fstream file(file_name);
        for(int k=0;k<num_keys/num_reduce;++k) {
            int key;
            int val;
            file >> key >> val;
            // 0 / 2 + 0
            // 1 / 2 + 0
            storage.emplace_back(key, val);
        }
        std::sort(storage.begin(), storage.end());
        for(int k=0;k<num_keys/num_reduce;++k) {
            int key = storage[k].first;
            int val = storage[k].second;
            EXPECT_EQ(key, k*num_reduce + r);
            EXPECT_EQ(val, num_val*num_producers);
        }
    }
} 

TEST(Reducer, MultMult) {
    int num_producers = 16;
    int num_reduce = 8;
    std::shared_ptr<vector<reduce_queue>>    reduce_ques(std::make_shared<vector<reduce_queue>>(num_reduce));
    UserReduce reduce_function;

    int num_keys = 800;
    int num_val = 100;
    {
        DoReduce reduction_process(num_reduce, reduce_ques, reduce_function);    
        // create two threads, each filling the queues with their own keys
        // first producer fills only even keys
        // second producers fills only odd keys
        vector<std::thread> producers;
        for (int i=0;i<num_producers;++i) {
            producers.emplace_back(&fill_data, num_reduce, num_keys, num_val, reduce_ques);
        }
        for(int i=0;i<num_producers;++i) {
            producers[i].join();
        }
    }
    // read the file and compare with desired values

    for (int r=0;r<num_reduce;++r) {
        vector<pair<int, int>> storage;
        string file_name = "reduction_" + std::to_string(r);
        std::fstream file(file_name);
        for(int k=0;k<num_keys/num_reduce;++k) {
            int key;
            int val;
            file >> key >> val;
            // 0 / 2 + 0
            // 1 / 2 + 0
            storage.emplace_back(key, val);
        }
        std::sort(storage.begin(), storage.end());
        for(int k=0;k<num_keys/num_reduce;++k) {
            int key = storage[k].first;
            int val = storage[k].second;
            EXPECT_EQ(key, k*num_reduce + r);
            EXPECT_EQ(val, num_val*num_producers);
        }
    }
}