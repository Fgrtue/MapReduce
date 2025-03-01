# MapReduce

This is my implementation of MadReduce programming model.

The description of high-level idea behind this programming model is presented below.

Let us first take a look at the structure of the project.

1. `/include'
           - `map-reduce-user.hpp' -- the most important file for the user, since they can define their Map and Reduce functions (classes), as well as all the needed classes
           - `reader.hpp` -- parsing part of the project
           - `do-map.hpp` -- file for the class with the map process
           - `do-reduce.hpp` -- file for the class with the reduce process
           - `con-queue.hpp` -- a file for a wrapper for the queue, that contains mutex and condition variable
           - `timer.hpp` -- class for benchmarking the execution of map-reduce process

2. `/src`
           - `do-map.cpp` -- defines constructor, destructor and method for working threads
           - `do-reduce.cpp` -- same as for map
           - `reader.cpp`  -- defines constructor and method `parse` for getting the data with the library
           - `error.cpp` -- a class for custom errors
           - `timer.cpp` -- empty file, no need to pay attention
3. `/test`
           - `test_map.cpp`, `test_reader.cpp`, `test_reduce.cpp` -- self explanatory
           - `word_count.cpp`, `sort.cpp` -- alternative direct algorithms without parallelization
           - all `.json' files -- are used as samples for tests

# How to build? 

# How does Reader work?

# How does Map and Reduce work?

# What do the benchmarks show?

# What is the original idea behind MapReduce programming model?

# What is there still to improve? 
