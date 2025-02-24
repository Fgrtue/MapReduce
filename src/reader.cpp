#include "reader.hpp"
#include "error.hpp"

// 1. Reader gets name of input file
// 2. We assume that input is in JSON format,
// -> vith "key" : value, where value is "string"
// 3. Save the value in a given format:
// -> first, create a key [we read data by chunks, so keys will be key1 .. keyN]
// -> read the data line by line
// -> each time return the number of characters in the line
// -> if you get that total size of strings is larger than
// -> some constant C, trunkate the string up to the next word
// -> then start making another chunk
// -> Keep the original key with the value, and push it to the queue
// 5. Return to queue to the main

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