#include "commons.hpp"

// This is a timer for benchmarking the time 
// for performing map and reduce
//  1. It has:
//      -> constructor that takes the time now
//      -> member begin
//      -> reset method for resetting the time
//      -> elapsed method that returns the differnce between
//          now and begin in seconds or miliseconds

struct Timer {

    using time_pt  = std::chrono::time_point<std::chrono::system_clock>;
    using duration = std::chrono::duration<double>;

    Timer()
    : start(std::chrono::system_clock::now())
    {}
    
    long long elapsed() {

        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start).count();
    }

    time_pt start;
};