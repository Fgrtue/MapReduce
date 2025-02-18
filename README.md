# MapReduce
This is my implementation of MadReduce programming model.


# Plan

1. Build MapReduce infrustructure that supports execution of user-defined Map() and Reduce() functions

We always have to preprocess data for map: 

Master()

-> Starts a reader
        Reader() -> opens the file that contains
                 a json with key value pairs
                 -> reads the keys and values as strings using reader threads
                 -> let reader thread divide values into smaller chunks in case the data is too big
                 -> Use CSV in order to separate the data into chunks of size of 1 KB or something like this (can be a constant)
                 -> It assignes each key a unique identifier

-> Master starts threads

-> It assignes M map tasks to M` threads

        Map() -> take input pair key1 value1
              -> perform some user defined operations to
              -> produce some number of key2 value2
              -> If list of values of key2 becomes too large
              -> we use combiner to combine them (if the option enabled)
              -> once produced some number of key2 value2 pairs
              -> put the key2 along with values to the Reducer queue (depending on the hash function)
              -> put them into the queue for Reducer (sort the values by reducer in adwace, to put them in batches)

-> It assignes R reduce tasks to R' threads
        Reduce() -> take input pair key2 value2
                 -> perform some taks to make key2 value
                 -> when finished with the queue
                 -> write all key values into some personal output file

Interaction between Map threads and Reduce threads happens using the queues in which Map threads put their results, and from which Reduce threads take their task 

2. Benchmark the 1) I/O phase
                 2) Map phase
                 3) Reduce phase
                 4) See where code can be optimized

## Plan 18.02

- Define Error class ✔️
- Provide definitions for Map and Reduce ✔️
- Declare Reader class ✔️
- Declare Do_Map class ✔️
- Declare Do_Reduce class ✔️ 
- Write a Make File ✔️

- Write ConQueue 
- Write Read class
- Test them
