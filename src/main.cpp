// This is the main file, where map reduce gets started

// 1. Read the input as the name of the file
// -> pass the value to reader
// -> let reader output key value pairs (hash map)
// 2. Depending on the total number of values, 
// -> decide how many map workers you would need
// 3. Use hash function to map each function to the queue
// -> for the mappers 
// 4. After all the tasks are assigned, go to starting reduce threads
// -> observe, that all the constructors must have been called by that
// -> time, meaning that there must be already the queues, where we shall put the
// tasks
// -> the key and value within a queue is defined by user when defining Map and Reduce
// functions
// 5. Then wait for all threads to finish
// -> the output will be saved in a number of files, each written by reducer
