// This is the main file, where map reduce gets started

#include "../include/commons.hpp"
#include "../include/reader.hpp"
#include "../include/timer.hpp"
#include "../include/error.hpp"

void Error(Err,const char*);

void do_word_count(queue<pair<string,string>>& jobs) {

    std::vector<int> vec;
    vec.reserve(1e7);
    while(!jobs.empty()) {
        auto job = jobs.front();
        jobs.pop();
        std::stringstream s(job.second);
        int num;
        while(s >> num) {
            vec.push_back(num);
        }
    }
    std::sort(vec.begin(), vec.end());
    std::ofstream file_reduce("sort_test", std::ios::out | std::ios::trunc);
    if (!file_reduce) {
        string msg = "failed create a file for word count ";
        Error(Err::REDUCE, &msg[0]);
    }
    for(auto& elem : vec) {
        file_reduce << elem << " " << 0 << "\n";
    }
}

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

    Reader reader(file_name);

    queue<pair<string,string>> jobs = reader.parse(); 
    
    // we will store vector of values for a specific key
    Timer timer_;
    {
        do_word_count(jobs);
    }
    auto elapsed = timer_.elapsed(); 
    std::cout << "Total time of direct sort " << elapsed << "µs"  << std::endl;
}
