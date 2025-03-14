#pragma once

#include "commons.hpp"

/*
    This queue is NOT intended to hold
    concurrency in its methods
    It is needed to provide mutex and 
    cond var for other classes that will
    use these queues in multithreded 
    environments

    Therefore, all its methods are trivial
*/

template<class T>
struct ConQueue {

    mutable std::mutex mt_;
    std::condition_variable cv_empty_;
    queue<T> data_;

    // Puts a value in the queue
    void push(T);

    // Pop the data from queue 
    T pop();

    // Check for emptiness
    bool empty();

    size_t size();

    ConQueue() = default;
    ConQueue(const ConQueue&) = delete; 
    ConQueue& operator=(const ConQueue&) = delete;
    ConQueue(ConQueue&&) = default;
    ConQueue& operator=(ConQueue&&) = default;
};

template<class T>
void ConQueue<T>::push(T val) {
    data_.push(std::move(val));
}

template<class T>
T ConQueue<T>::pop() {
    T val = data_.front();
    data_.pop();
    return val;
}

template<class T>
bool ConQueue<T>::empty() {
    return data_.empty();
}

template<class T>
size_t ConQueue<T>::size() {
    return data_.size();
}
