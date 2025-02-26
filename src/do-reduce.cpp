#include "do-reduce.hpp"

// 1. Reduce class has a:
//      -> Constructor that takes an int, a hash function
//      -> and vector with ConQueues
//      -> variable atomic map_finish
//      -> Vector with reducers
//      -> Destructor
// 2. Constructor
//      -> Creates Reducers threads, calling a constructor for reduce
//      -> providing every reducer its queue for jobs
// 3. Destructor
//      -> takes the lock to the map_finish
//      -> sets map_finish
//      -> walks through all the queues, gets the lock and wakes up all threads
//      -> Joins all reducer threads
// class ReduceWorker
// 1. Receives queue into constructor
//      -> creates a file with a name reduce_i
//      -> In a while loop 
//          -> takes the mutex
//          -> it checks the work_finish variable
//          -> checks the emptiness of the queue
//          -> sleeps on cv in case the queue is empty
//         -> it checks if queue is empty
//              -> if not, takes and pops the next value
//              -> Releases the lock
//              -> Uses Reduce() (defined by user)
//              -> writes to the file that it created
//         -> takes the lock 
//         ->  checks map_finish variable
//              -> returns if it is true

void Error(Err er_num, const char* msg);

thread_local UserReduce DoReduce::reduce_func_;

DoReduce::DoReduce(int num_reducers, const std::shared_ptr<vector<reduce_queue>>& reduce_ques)
             : num_reducers_(num_reducers)
             , vec_map_finished_(num_reducers_)
             , reduce_ques_(reduce_ques)
{   
    for(int i=0;i<num_reducers;++i) {
        vec_map_finished_[i].store(false);
        rdsrs_.emplace_back(&DoReduce::reduction_worker, this, i, std::ref((*reduce_ques)[i]), std::ref(vec_map_finished_[i]));
    }
}

DoReduce::~DoReduce() {

    for(int i=0;i<num_reducers_;i++) {
        reduce_queue& q = (*reduce_ques_)[i];
        std::lock_guard lg_(q.mt_);
        vec_map_finished_[i].store(true);
        q.cv_empty_.notify_one();
    }
    for(int i=0;i<num_reducers_;i++) {
        rdsrs_[i].join();
    }
}

void DoReduce::reduction_worker(int num_rds, reduce_queue& q, std::atomic<bool>& map_fin) {

    string name_file = "reduction_" + std::to_string(num_rds);
    std::ofstream file_reduce(name_file, std::ios::out | std::ios::trunc);
    if (!file_reduce) {
        string msg = "failed create a file for reducer " + std::to_string(num_rds);
        Error(Err::REDUCE, &msg[0]);
    }

    std::map<UserReduce::Key, UserReduce::Value> storage;
    while(true) {

        std::unique_lock<std::mutex> ul_(q.mt_);
        // wait on cv, in case map is still doing its work
        // and q is not empty
        while(!map_fin.load() || q.empty()) {
            q.cv_empty_.wait(ul_);
        }
        while (!q.empty()) {
            auto p = q.pop();
            auto& key = p.first;
            auto& val = p.second;
            ul_.unlock();
            auto output = reduce_func_(key, val);
            storage[output.first] += output.second;
            ul_.lock();
        }
        if (map_fin.load()) {
            break;
        }
    }
    for(auto& [key,value] : storage) {
        file_reduce << key << " " << value << "\n";
        file_reduce.flush();
    }
}