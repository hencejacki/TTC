#ifndef TASK_HPP
#define TASK_HPP

#include <iostream>
#include <string>
#include <chrono>
#include <unordered_map>
#include <cstdarg>
#include <vector>

struct Task {
    std::string id;
    std::string description;
    std::string status;
    std::string created_at;
    std::string updated_at;
};

static std::unordered_map<std::string, uint8_t> support_cmd = {
    {"add"              , 3},
    {"update"           , 4},
    {"delete"           , 3},
    {"mark-in-progress" , 3},
    {"mark-done"        , 3},
    {"list"             , 2}
};

static inline void ShowUsage(const std::string& prog_name) {
    std::cout
        << "Usage:\r\n"
        << prog_name << " add [task description]\r\n"
        << prog_name << " update [task id] [task description]\r\n"
        << prog_name << " delete [task id]\r\n"
        << prog_name << " mark-in-progress [task id]\r\n"
        << prog_name << " mark-in-done [task id]\r\n"
        << prog_name << " list [done|todo|in-progress]\r\n";
}

static inline void ErrIf(bool cond, const char* fmt, ...) {
    if (cond) {
        std::va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        printf("\n");
        va_end(args);
        exit(1);
    }
}

#endif // TASK_HPP