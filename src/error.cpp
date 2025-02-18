#include "../include/error.hpp"

void Error(Err err_num, const char* msg) {
    std::string message = std::to_string(static_cast<int>(err_num)) + ": " + std::string(msg);
    throw std::runtime_error(message);
}