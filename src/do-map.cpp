#include "do-map.hpp"

thread_local UserMap DoMap::map_func_; 
thread_local deque<pair<UserReduce::Key, UserReduce::Value>>     DoMap::store;
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
        auto& q = (*map_queues_)[i];
        std::lock_guard<std::mutex> lg_(q.mt_);
        work_to_do_[i] = chunk_of_work + (i < remainder_work ? 1 : 0);
        map_workers_.emplace_back(&DoMap::map_worker, this, i);
    }

    for(int job_num = 0; job_num < total_work; ++job_num) {
        int worker_ind = (job_num % num_map_);
        auto& q = (*map_queues_)[worker_ind];
        std::lock_guard lg_(q.mt_);
        q.push(queue_jobs.front());
        q.cv_empty_.notify_one();
        queue_jobs.pop();
    }

}

DoMap::~DoMap() {

    for(int i=0; i<num_map_;++i) {
        auto& q = (*map_queues_)[i];
        std::unique_lock ul_(q.mt_);
        q.cv_empty_.notify_all();
    }
    for(int i=0; i<num_map_;++i) {
        map_workers_[i].join();
        assert(work_to_do_[i] == 0);
    }
}

void DoMap::map_worker(int w) {

    const int MAX_SZ = 2048;
    key_vvalues.reserve(MAX_SZ);
    ready_send.resize(num_reducers_);
    for(int i=0; i < num_map_;++i) {
        int ind = (w + i) % num_map_;
        auto& q = (*map_queues_)[ind];
        while (true) {

            std::unique_lock ul_(q.mt_);
            q.cv_empty_.wait(ul_, [&] { return work_to_do_[ind] == 0 || !q.empty(); });
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
                ul_.unlock();
                Send(store);
                break;
            }
        }

    }
}

void DoMap::Send(deque<pair<UserReduce::Key, UserReduce::Value>>& store) {
    if (store.empty()) return ;

    while(!store.empty()) {
        // 1. put kets inside the key_values
        auto elem = store.front();
        store.pop_front();
        key_vvalues[elem.first].push_back(std::move(elem.second));
    }
    // 2. for each key find out where it is from and insert them into the ready_send
    for(auto& it : key_vvalues) {
        int key_num = hash_reduce_(it.first);
        ready_send[key_num].push_back(std::move(it));
        it.second.clear(); 
    } 
    // 3. then send the values to the reducer for each reducer one by one
    for(int rdsr = 0; rdsr < num_reducers_;++rdsr) {
        if (ready_send[rdsr].empty()) continue; 
        auto& q = (*reduce_queues_)[rdsr];
        std::lock_guard<std::mutex> lg(q.mt_);
        while(!ready_send[rdsr].empty()) {
            q.push(std::move(ready_send[rdsr].back()));
            ready_send[rdsr].pop_back();
        }
        q.cv_empty_.notify_one();
    }
}