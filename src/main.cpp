// This is the main file, where map reduce gets started

#include "commons.hpp"
#include "reader.hpp"
#include "do-map.hpp"
#include "do-reduce.hpp"
#include "map-reduce-user.hpp"
#include "con-queue.hpp"
#include "error.hpp"

// 1. Read the input as the name of the file
// -> pass the value to reader
// -> let reader output key value pairs (hash map)
// 2. Depending on the total number of values, 
// -> decide how many map workers you would need
// -> Create the vector of ConQueues for each map worker
// -> Compute the number of reducers 
// -> Create a vector of ConQueues for reducers
//      -> let the number be something like a nultiple of tasks
//      -> We'd better have less map workers than tasks
// 3. Create Reduce class, providing it
//      -> number of reducers
//      -> vector of Conqueue for Reduce workers
//      -> hash function for Reducers
// 4. Create Map class, providing it 
//      -> number of mappers, 
//      -> queue of tasks,
//      -> vector of ConQueue for Map workers
//      -> vector of Conqueue for Reduce workers
//      -> hash function for Map
//      -> hash function for Reducers
// !Observer that it is important to create Reducers before Map
// Since it will allow to destroy Map before Reducers (using LIFO order)
//  That would allow us to make sure that all Map workers are done when we
//  Wake up all threads in Reduce before destruction!
// -> the output will be saved in a number of files, each written by reducer

void Error(Err,const char*);

constexpr double map_coefficient = 10;
constexpr double reduce_coefficient = 2.5;
constexpr int    number_of_cores  = 8;
constexpr int    parallelism_map  = static_cast<int>(map_coefficient * number_of_cores);
constexpr int    parallelism_reduce  = static_cast<int>(reduce_coefficient * number_of_cores);

int main(int argc, char* argv[]) {

    if (argc != 2) {
        // Create error class
        Error(Err::PARSING, "must be 2 arguments");
    }
    string file_name{argv[1]};
    // check that file name is in .json format
    if (file_name.size() <= 5 || file_name.substr(file_name.size() - 5, 5) != ".json") {
        Error(Err::PARSING, "file must be in .json format");
    }

    // Hash Function
    auto hash_reducer = [](const UserReduce::Key& key) {
        thread_local std::hash<UserReduce::Key> hasher;
        return hasher(key) % parallelism_reduce;
    };

    Reader reader(file_name);

    queue<pair<string,string>> jobs = reader.parse(); 
    
    // we will store vector of values for a specific key
    std::shared_ptr<vector<reduce_queue>>    reduce_queues(std::make_shared<vector<reduce_queue>>(parallelism_reduce));
    std::shared_ptr<vector<map_queue>>      map_queues(std::make_shared<vector<map_queue>>(parallelism_map));

    DoReduce reduction_process(parallelism_reduce, reduce_queues);
    DoMap map_process(parallelism_map, parallelism_reduce, jobs, map_queues, reduce_queues, hash_reducer);
}
