#include "reader.hpp"
#include "error.hpp"

void Error(Err,const char*);

Reader::Reader(const string& s) {
    input_file.open(s, std::ios_base::in);
    if (!input_file.is_open()) {
            Error(Err::PARSING, "couldn't open the file");
    }
}

queue<pair<string,string>> Reader::parse() {

    queue<pair<string,string>> res; 
    json content;
    try {
        content = json::parse(input_file);
    } catch (const std::runtime_error& e) {
        Error(Err::PARSING, e.what());
    }

    // stucture binding available in nlohmann's json
    for(auto& [key, value] : content.items()) {
        // Check that the value is string
        if (value.is_string()) {
            size_t l = 0;
            while(l < static_cast<string>(value).size()) {
                size_t sz = choose_size(l, value);
                res.emplace(key, static_cast<string>(value).substr(l, sz));
                l += sz;
            }       
        } else {
            Error(Err::PARSING, "values must be of string type");
        }
    }
    
    return res;
}

size_t Reader::choose_size(size_t l, const string& str) {

    size_t r = l + CHUNK_SIZE;
    while(r < str.size() && !isspace(str[r])) {
        ++r;
    }
    return r - l;
}