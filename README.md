# MapReduce

This is my implementation of MadReduce programming model.

The description of the high-level idea behind this programming model is presented below.

Let us first take a look at the structure of the project.

1. `/include`
   - `map-reduce-user.hpp` -- the most important file for the user, since they can define their Map and Reduce functions (classes), as well as all the needed classes
   - `reader.hpp` -- parsing part of the project
   - `do-map.hpp` -- file for the class with the map process
   - `do-reduce.hpp` -- file for the class with the reduce process
   - `con-queue.hpp` -- a file for a wrapper for the queue, that contains mutex and condition variable
   - `timer.hpp` -- class for benchmarking the execution of map-reduce process

2. `/src`
   - `do-map.cpp` -- defines constructor, destructor, and method for working threads
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

In fact, the user needs to define their Map and Reduce functions before even building the project. There might be some restrictions related to that since when I created the project, I assumed only simple types to be used as output for Map function: such as strings or integers. Moreover, inside Reduce function I use the `+` operation when summing up the results for a particular key after performing Reduce (since there might be multiple Map functions, that send <Key2,vec<Value2>> to the same reducer). Therefore, it is important to ensure that for Value2 operation `+` is defined in such a way that we obtain the correct result. Value2 and Key2 must also be printable using streams.  

# How does main work?

It ensures that there was a second argument for `main` is provided. It then checks that the file provided is of the right extension. Then it parses the data and runs first `DoReduce` and then `DoMap` in a separate scope. This choice is made as such so that we can collect all the threads in the destructors of the classes. Since they are put on the stack, the first destructor for `DoMap` will be called, and then for `DoReduce`. Which is exactly as we expect. Moreover, we benchmark the code inside the mentioned scope and print the result in microseconds.

# How does Reader work?

The Reader does a pretty simple job. It first ensures that the file can be opened, and it also has a method for parsing the `.json` file provided by the user. NOTE: this is important that all the values in `.json` file are just strings, where each element is separated from each other with a space character. This choice was made, since in the original version of MapReduce, the user provides just some files with keys and values, that, as far as I concluded, can be presented as strings and be parsed in the map function. Then, Reader just outputs the queue with jobs presented as key-value pairs. While parsing reader divides each string into smaller chunks, in case it exceeds the maximum size. This allows us to keep the granularity of tasks. 

# How does Map and Reduce work?

As was described in the original paper, we have a master and workers. In our case, each of them is a thread. The master thread in the constructor of DoMap and DoReduce creates the corresponding workers. For the full picture, we got to look at all three functions for each class. Let us start with DoReduce as from a simpler class.

### DoReduce

1. Constructor
   - In the constructor, we just create the threads along with booleans for each reduce that marks the point when map functions finish their work. This is needed, since we don't know in advance how much work will there be for each reduce worker. However, we know that once map workers are done with their work, we can safely wake up each reducer to wrap things up.
2. Destructor
   - In destructor, as we mentioned, we just set that maps threads are done, wake up the thread in case it was sleeping, and then join all the threads together
3. Reduction worker
   - It works as a simple synchronization mechanism for multiple producers, in a single consumer queue. Modulo the fact that we finish only when all the reducers did their jobs.
  
### DoMap

The key difference between DoMap and DoReduce is that for map workers we can know exactly how many tasks each of them must perform. This information is computed in the constructor of DoMap. Each worker thread first performs all the tasks in its primary queue of jobs (this is the queue with the same number as the number of workers), and then helps other threads to finish the work in queues. This enables the feature that is mentioned in the paper as Backup Tasks.

# How do tests work?

We have tests for: 1) Reader 2) DoMap 3) DoReduce.
I think that is not so interesting to walk through the test cases themself. If you are interested, you can walk through the source files to see the description of test cases. So I will describe the motivation for a general idea for the tests. First of all, I tried to check whether DoMap and DoReduce can perform on different levels of concurrency. I also checked Reader for its ability to handle errors during parsing. 

# What do the benchmarks show?

We have two cases, that were looked up in the paper "MapReduce: Simplified Data Processing on Large Clusters" by J.Dean and S.Ghemawat. The first one is word count: we have a number of texts, in which we want to count words. Therefore, the input for the map is:
   - `key`: name of a document
   - `value`: document contents
The output of reduce is:
   - `key`: a word
   - `value`: number of such words
In our example, we took `20` texts `Lorem ipsum dolor` with `9999` words each. We used a simple program for counting words, that can be found in `test/word_count.cpp` in order to compare the performance of parallel map-reduce with something. We build both files using `Release` mode.

After carefully running 100 times both `word_count` and `map-reduce` binaries, the results were uplifting: the average execution time for `map-reduce` was  `12'793 µs` versus  `16'599 µs` for `word_count`. 

The same was done was `sort` example, where I generated `1e6` numbers from `0` to `1e6`. In `test/sort.cpp` can be seen that we just use `std::sort` algorithm for sorting all the numbers, while our `map-reduce` generates `4` files (`1` file per reducer), each of which is sorted. Therefore, by merging these data we can consider the task to be solved. Thus, after again carefully running 10 times `sort` and `map-reduce`, we obtained the following on average: for sequential sort`1'229'647 µs` versus `920'909 µs` for map-reduce.

In fact, the results can be even better. It depends on what in the paper is named Network bandwidth. In our case, this is the constant `MAX_SZ` in the map worker. This constant tells when each worker must send the data to the reduce function. It appeared that this constant is crucial for the optimal performance of the program. This constant in fact should be a variable that depends on the number of different key-value pairs. Since the less frequently we send the data, the better the performance is. However, if we do this too rarely, we can also get suboptimal performance. The best way would be to allow this value to be defined by the user, depending on the size of input that they are willing to provide. However, for now, I decided to leave this number as a constant, since I am not entirely sure what parameters this value depends on and how it should be computed. 

# What is the original idea behind MapReduce programming model?

Here we briefly explain the programming model described in "MapReduce: Simplified Data Processing on Large Clusters" by J.Dean and S.Ghemawat. For more details please check the original paper. Thus, computation takes a set of input key/value pairs and produces a set of output key/value pairs. The user-defined Map and Reduce. 

Map, written by the user, takes an input pair and produces a set of intermediate key/value pairs. Then MapReduce library groups together all intermediate values associated with the same intermediate key I and passes them to the Reduce function. 

The Reduce function, also written by the user, accepts the intermediate key I and a set of values for that key. It merges together these values to form a possibly smaller set of values. Typically zero or one output value is producer per Reduce invocation. 

Functionally: 

   - `map (k1,v1) -> list(k2,v2)`
   - `reduce (k2,list(v2)) -> list(v2)`

# What is there still to improve? 

First, it would be good to make the program work even faster. There are two ways for improvement: a) choose the best-suited constant for sending data from map worker to reduce worker, b) enable more concurrency. However, the second option might require a change in other constants, especially in the *network bandwidth* constant (mentioned in a) item).

Another part that was not implemented is related to the failure of a `Map` worker. In the original paper, in case one of the workers fails, others pick up the jobs that it finished, and redo them. We did not implement this part. However, it doesn't seem difficult to do. 

Combiner function within Map. Basically, this is a reduce function within the map function, used for extremely large datasets. This might be a valuable improvement, that could lead to better average performance. 


