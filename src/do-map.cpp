#include "do-map.hpp"

// 1. class Map has:
// -> Constructor() that takes integer
// -> And a queue with all the work, where first element is a number
//      -> and second element is a queue with pair <string, string>
//      -> that are key values provided to the map worker
//      -> we need to make sure how much work does each worker receive
// -> member vector of workers 
// -> method "Do_Work()"
// -> Destructor 
//      -> joins them
// 2. Constructor
//  -> Takes integer as input, as well as the queue with work,
//      -> ConQueues for map
//      -> ConQueues for reducers
//      -> hash function for mapper
//      -> hash function for reducers
//  -> It creates a vector of workers
//          -> each worker is a thread
//          -> We provide it the above vectors of queues
//                 -> and index of the thread (for getting the first queue)
//          -> Each worker has a storage for saving finished work
//          -> and a method for Work()
//             -> which inside uses fuction Map() defined by user
//          -> and a method for Send()
//  3. Do_work()
//  -> The main thread takes jobs from general queue of tasks and fills queues for each worker
//      -> using hash function for mappers
//  4. Destructor
//  -> In the destructor we have to wait for all the threads
//  -> and join them!

// Let us talk a bit about the Worker function
//      -> and an index to its number
//      -> also takes the hash function
//      -> Member queue for storing the results
//      -> the maximum number of task to start sending them
//      -> a hash function for communucating with Reducers
//             -> along with a number of reducers
//      -> We also have an atomic counter for number of tasks finished
//      -> Method Sort() -- we don't actually need the method sort, 
//                          since they will be all sorted in Reduce
//      -> Method Send() 
//      -> Method Work()
//      -> Method Map() (defined by user)
// 2. Constructor
///     -> It takes reference to vector of queues
//      -> And index
//      -> For loop
//                  -> in which we start from index, and increase an index
//                  -> until we reach outself (use modulo operation)
//          -> In which we Work with the queue to which we are
//          -> pointing at the moment
// 3. Method Work(takes queue as input)
//      while (true)
//      -> 1. Take the mutex to the queue
//      -> 2. While work_done.load() < work_total && queue is empty
//              -> If so, then wait
//      -> 3.  if work_done.load() == work_total()
//          -> i.e. we left the loop because of the second cond
//          -> send all the work left, and return ;
//      -> 4. If not, then pop element from the queue
//      -> 5. Release lock
//      -> 6. Do Map() taks
//      -> 7. Then check the work_load 
//          -> i.e. how many tasks saved in our storage
//          -> if it became equal to the max capacity
//          -> Send()
//      -> 8. Get the lock
//      -> 9. if (fetch_add(work_done) == work_total)
//          -> wake up all the threads waiting on CV
//          -> release the lock
//          -> Send()
// 4. Method Send()
//      -> Sort() elements in the storage // don't need this step
//      -> While non empty the storage
//      -> Take the first element, pop it
//      -> Get the queue of the needed reducer (using key and hash)
//              -> probably via providing the
//              -> reducers vector inside the constructor?
//      -> Push in the queue of reducer
// 
// ConQueue
// 1. Struct that has
// -> member queue
// -> member lock
// -> member condition variable
// -> Method Push
// -> Method Pop
// -> Method Empty
// 2. We assume that usage of every method push/pop/empty
// -> happens under the lock
// -> the methods are thus trivial


/*

TO IMPROVE: 
     1. For main thread allow it to push first 1/3 of work, and then push back the last 2 
     2. Use move semantics where possible
     3. Do not iterate with values, but with references
     4. Try to optimize clearing (actually probably by using move semantics)
     5. Try to reserve space for threads (for key_value stores, for ready send and for store)
*/

thread_local UserMap DoMap::map_func_; 
thread_local deque<pair<UserReduce::Key, UserReduce::Value>>    DoMap::store;
thread_local std::unordered_map<UserReduce::Key, vred_val>       DoMap::key_vvalues;
thread_local vector<vector<pair<UserReduce::Key, vred_val>>>     DoMap:: ready_send;

DoMap::DoMap(int num_map, int num_reduce, queue<pair<string,string>>& queue_jobs, 
            const std::shared_ptr<vector<map_queue>>& map_queues, 
            const std::shared_ptr<vector<reduce_queue>>& reduce_queues, 
            std::function<int(UserReduce::Key)> hash_reduce) 
: num_map_(num_map)
, num_reducers_(num_reduce)
, map_queues_(map_queues)
, reduce_queues_(reduce_queues)
, hash_reduce_(std::move(hash_reduce))
, work_to_do_(num_map_)
{
    size_t total_work = queue_jobs.size();
    size_t chunk_of_work = (num_map_ ? total_work / num_map_ : 0);
    size_t remainder_work = (num_map ? total_work % num_map_ : 0);

    for(int i=0;i<num_map;++i) {
        work_to_do_[i] = chunk_of_work + (i < remainder_work ? 1 : 0);
        map_workers_.emplace_back(&DoMap::map_worker, this, i);
    }

    for(int job_num = 0; job_num < total_work; ++job_num) {
        auto job = queue_jobs.front();
        queue_jobs.pop();
        int worker_ind = (job_num % num_map_);
        auto& q = (*map_queues_)[worker_ind];
        std::lock_guard lg_(q.mt_);
        q.push(std::move(job));
        q.cv_empty_.notify_one();
    }
}

DoMap::~DoMap() {

    for(int i=0; i<num_map_;++i) {
        auto& q = (*map_queues_)[i];
        std::unique_lock ul_(q.mt_);
        q.cv_empty_.notify_all();
        ul_.unlock();        
    }
    for(int i=0; i<num_map_;++i) {
        map_workers_[i].join();
        assert(work_to_do_[i] == 0);
    }
}

void DoMap::map_worker(int w) {

    const int MAX_SZ = 512;
    key_vvalues.reserve(MAX_SZ);
    ready_send.resize(num_reducers_);
    for(int i=0; i < num_map_;++i) {
        int ind = (w + i) % num_map_;
        auto& q = (*map_queues_)[ind];
        while (true) {

            std::unique_lock ul_(q.mt_);
            while(work_to_do_[ind] != 0 && q.empty()) {
                q.cv_empty_.wait(ul_);
            }
            while(!q.empty()) {
                auto job = q.pop();
                work_to_do_[ind]--;
                ul_.unlock();
                auto output = map_func_(job.first, job.second);
                store.insert(store.end(), output.begin(), output.end());
                if(MAX_SZ <= store.size()) {
                    Send(store);
                }
                ul_.lock();
            }
            if(work_to_do_[ind] == 0) {
                q.cv_empty_.notify_all();
                Send(store);
                break;
            }
        }

    }
}

void DoMap::Send(deque<pair<UserReduce::Key, UserReduce::Value>>& store) {
    while(!store.empty()) {
        // 1. put kets inside the key_values
        auto elem = store.front();
        store.pop_front();
        key_vvalues[elem.first].push_back(elem.second);
    }
    // 2. for each key find out where it is from and insert them into the ready_send
    for(auto& it : key_vvalues) {
        int key_num = hash_reduce_(it.first);
        ready_send[key_num].push_back(it);
        it.second.clear();    
    } 
    // 3. then send the values to the reducer for each reducer one by one
    for(int rdsr = 0; rdsr < num_reducers_;++rdsr) {
        if (ready_send[rdsr].empty()) continue; 
        auto& q = (*reduce_queues_)[rdsr];
        std::lock_guard(q.mt_);
        while(!ready_send[rdsr].empty()) {
            q.push(ready_send[rdsr].back());
            ready_send.pop_back();
        }
        q.cv_empty_.notify_one();
    }

}