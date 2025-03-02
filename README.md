# MapReduce

This is my implementation of MadReduce programming model.

The description of high-level idea behind this programming model is presented below.

Let us first take a look at the structure of the project.

1. `/include`
   - `map-reduce-user.hpp` -- the most important file for the user, since they can define their Map and Reduce functions (classes), as well as all the needed classes
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
   - all `.json` files -- are used as samples for tests

# How to build? 

To build the project you need a library for convenient and easy parsing of `.json` files by Nils Lohman, which is called `nlohmann-json`. It can be found here: 

`https://github.com/nlohmann/json`

After building this library, rename the folder `nlohmann_json` so the dependencies are correct in CMakeLists.txt. 

Then do the following in the root of the directory:

```
> mkdir build
> cd build
> cmake -DCMAKE_BUILD_TYPE=Release .. // or =Debug if you want to test
> cmake --build . 
```

This will make all the files in the project. The one that you might be in particularly interested in is `map-reduce`.

# How to use?

In fact, before even building the project the user needs to define their Map and Reduce functions. There might be some restrictions related to that since when I created the project, I assumed only simple types to be used as output for Map function. Such as strings or integers. Moreover, inside Reduce function I use the `+` operation when summing up the results for a particular key after performing Reduce (since there might be multiple Map functions, that send <Key2,vec<Value2>> to the same reducer). Therefore, it is important to ensure that for Value2 operation `+` is defined in such a way that we obtain the correct result. Value2 and Key2 must also be printable using streams.  

# How does main work?

It ensures that there was a second argument for main provided. It then checks that the file provided is of the right extension. Then it parses the data and runs first `DoReduce` and then `DoMap` in a separate scope. This choice for made in such a way that we can collect all the threads in the destructors of the classes. Since they are put on the stack, the first destructor for `DoMap` will be called, and then for `DoReduce`. Which is exactly as we expect. Moreover, we benchmark the code inside the mentioned scope and print the result in microseconds.

# How does Reader work?

The Reader does a pretty simple job. It first ensures that the file can be opened, and it also has a method for parsing the `.json` file provided by the user. NOTE: this is important that all the values in `.json` file are just strings. This choice was made, since in the original version of MapReduce, the user provides just some files with key value, that, as far as I concluded, can be presented as strings. Then reader just outputs the queue with jobs presented as key value pairs.

# How does Map and Reduce work?

As was described in the original paper, we have a master and workers. In our case, each of them is a thread. Master thread in the constructor of DoMap and DoReduce creates the corresponding workers. For the full picture we got to look at all three functions for each class. Let us start with DoReduce as from a simpler class.

### DoReduce

1. Constructor
   - In the constructor, we just create the threads along with booleans for each reduce that marks the point when map functions finish their work. This is needed, since we don't know in advance how much work will there be for each reduce worker. However, we know that once map workers are done with their work, we can safely wake up each reducer to wrap things up.
2. Destructor
   - In destructor, as we mentioned, we just set that maps threads are done, wake up the thread in case it was sleeping, and then join all the threads together
3. Reduction worker
   - It works as a simple synchronization mechanism for multiple producers, a single consumer queue. Modulo the fact that we finish only when all the reducers did their jobs.

# How do tests work?



# What do the benchmarks show?

# What is the original idea behind MapReduce programming model?

# What is there still to improve? 
