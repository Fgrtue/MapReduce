#pragma once

#include <nlohmann/json.hpp>
#include "commons.hpp"

using json = nlohmann::json;

/*

    Class Reader is destined to be used
    for parsing the data

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