// This is the main file, where map reduce gets started

// 1. Read the input as the name of the file
// -> pass the value to reader
// -> let reader output key value pairs (hash map)
// 2. Depending on the total number of values, 
// -> decide how many map workers you would need
// -> Create the vector of ConQueues for each map worker
// -> Compute the number of reducers 
// -> Create a vector of ConQueues for reducers
//      -> let the number be something like a nultiple of tasks
//      -> We'd better have less map workers than threads
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
