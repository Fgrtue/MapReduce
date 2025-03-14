#pragma once

#include "commons.hpp"
#include "con-queue.hpp"
#include "map-reduce-user.hpp"

/*

    1. class Map has:
    -> Constructor() that takes integer
    -> And a queue with all the work, where first element is a number
        -> and second element is a queue with pair <string, string>
        -> that are key values provided to the map worker
        -> we need to make sure how much work does each worker receive
    -> member vector of workers 
    -> method "Do_Work()"
    -> Destructor 
        -> joins them
    2. Constructor
    -> Takes integer as input, as well as the queue with work,
        -> ConQueues for map
        -> ConQueues for reducers
        -> hash function for mapper
        -> hash function for reducers
    -> It creates a vector of workers
            -> each worker is a thread
            -> We provide it the above vectors of queues
                    -> and index of the thread (for getting the first queue)
            -> Each worker has a storage for saving finished work
            -> and a method for Work()
                -> which inside uses fuction Map() defined by user
            -> and a method for Send()
    3. Do_work()
    -> The main thread takes jobs from general queue of tasks and fills queues for each worker
        -> using hash function for mappers
    4. Destructor
    -> In the destructor we have to wait for all the threads
    -> and join them!

    Let us talk a bit about the Worker function
        -> and an index to its number
        -> also takes the hash function
        -> Member queue for storing the results
        -> the maximum number of task to start sending them
        -> a hash function for communucating with Reducers
                -> along with a number of reducers
        -> We also have an atomic counter for number of tasks finished
        -> Method Sort() -- we don't actually need the method sort, 
                            since they will be all sorted in Reduce
        -> Method Send() 
        -> Method Work()
        -> Method Map() (defined by user)
    2. Constructor
    /     -> It takes reference to vector of queues
        -> And index
        -> For loop
                    -> in which we start from index, and increase an index
                    -> until we reach outself (use modulo operation)
            -> In which we Work with the queue to which we are
            -> pointing at the moment
    3. Method Work(takes queue as input)
        while (true)
        -> 1. Take the mutex to the queue
        -> 2. While work_done.load() < work_total && queue is empty
                -> If so, then wait
        -> 3.  if work_done.load() == work_total()
            -> i.e. we left the loop because of the second cond
            -> send all the work left, and return ;
        -> 4. If not, then pop element from the queue
        -> 5. Release lock
        -> 6. Do Map() taks
        -> 7. Then check the work_load 
            -> i.e. how many tasks saved in our storage
            -> if it became equal to the max capacity
            -> Send()
        -> 8. Get the lock
        -> 9. if (fetch_add(work_done) == work_total)
            -> wake up all the threads waiting on CV
            -> release the lock
            -> Send()
    4. Method Send()
        -> Sort() elements in the storage // don't need this step
        -> While non empty the storage
        -> Take the first element, pop it
        -> Get the queue of the needed reducer (using key and hash)
                -> probably via providing the
                -> reducers vector inside the constructor?
        -> Push in the queue of reducer

    ConQueue
    1. Struct that has
    -> member queue
    -> member lock
    -> member condition variable
    -> Method Push
    -> Method Pop
    -> Method Empty
    2. We assume that usage of every method push/pop/empty
    -> happens under the lock
    -> the methods are thus trivial

*/

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

    void Send();

    void map_worker(int);

public:

    DoMap(int, int, queue<pair<string,string>>&, 
            const std::shared_ptr<vector<map_queue>>&, 
            const std::shared_ptr<vector<reduce_queue>>&, 
            std::function<int(UserReduce::Key)>);

    ~DoMap();
};