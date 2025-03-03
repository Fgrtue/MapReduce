// This is the main file, where map reduce gets started

#include "../include/commons.hpp"
#include "../include/reader.hpp"
#include "../include/timer.hpp"
#include "../include/error.hpp"

void Error(Err,const char*);

void do_word_count(queue<pair<string,string>>& jobs) {

    std::unordered_map<std::string, int> store;
    while(!jobs.empty()) {
        auto job = jobs.front();
        jobs.pop();
        auto value = std::move(job.second);
        string word;
        for(size_t i=0;i<value.size();++i) {
            if (!std::isspace(value[i]) && !std::ispunct(value[i])) {
                word.push_back(std::tolower(value[i]));
            } else {
                store[word]++;
                word.clear();
            }
        }
        if (!word.empty()) store[word]++;
    }
    std::ofstream file_reduce("word_count_test", std::ios::out | std::ios::trunc);
    if (!file_reduce) {
        string msg = "failed create a file for word count ";
        Error(Err::REDUCE, &msg[0]);
    }
    for(auto& [key,value] : store) {
        file_reduce << key << " " << value << "\n";
        file_reduce.flush();
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
    std::cout << "Total time of direct word count " << elapsed << "µs"  << std::endl;
}
