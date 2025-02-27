#pragma once

#include <nlohmann/json.hpp>
#include "commons.hpp"

using json = nlohmann::json;

/*

    Class Reader is destined to be used
    for parsing the data

    How is works:

    1. Reader gets name of input file
    2. We assume that input is in JSON format,
    -> vith "key" : value, where value is "string"
    3. Save the value in a given format:
    -> first, create a key [we read data by chunks, so keys will be key1 .. keyN]
    -> read the data line by line
    -> each time return the number of characters in the line
    -> if you get that total size of strings is larger than
    -> some constant C, trunkate the string up to the next word
    -> then start making another chunk
    -> Keep the original key with the value, and push it to the queue
    5. Return to queue to the main

*/

class Reader {

private:

    size_t choose_size(size_t, const string&);

    static constexpr size_t CHUNK_SIZE = 1024;
    std::fstream input_file;

public:

    // Try open the file
    // Catch and throw custom error 
    // if not working
    Reader(const string&);

    // Read the content of the file
    // By splitting them in chunks of 1KB
    queue<pair<string,string>> parse();

};