
// 1. Reader gets name of input file
// 2. We assume that input is in JSON format,
// -> vith "key" : value, where value is either "string" or number
// 3. Save the value in a given format:
// -> first, create a key [we read data by chunks, so keys will be key1 .. keyN]
// -> read the data line by line
// -> each time return the number of characters in the line
// -> if you get that total size of strings is larger than
// -> some constant C, then start making another chunk with
// -> key_i+1
// -> IT is actually better not to lose the original key, and
// -> keep in hash map key,value pair 
// 5. Return to queue to the main