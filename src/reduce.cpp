
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
//      -> sets work_finish
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