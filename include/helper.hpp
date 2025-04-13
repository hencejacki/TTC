#ifndef TASK_HPP
#define TASK_HPP

#include <iostream>
#include <string>
#include <chrono>
#include <unordered_map>
#include <map>
#include <cstdarg>
#include <vector>
#include <sstream>
#include <iomanip>
#include "err.hpp"

using SystemClock = std::chrono::system_clock;
using SystemTimePoint = std::chrono::time_point<SystemClock>;

const char* kTaskDataBaseName = "task.json";

struct Task {
    std::string id;
    std::string description;
    int         status;
    std::string created_at;
    std::string updated_at;
};

const std::string kAddCmd        = "add";
const std::string kUpdateCmd     = "update";
const std::string kDeleteCmd     = "delete";
const std::string kMarkProgCmd   = "mark-in-progress";
const std::string kMarkDoneCmd   = "mark-done";
const std::string kListCmd       = "list";

static std::unordered_map<std::string, uint8_t> support_cmd = {
    {kAddCmd              , 3},
    {kUpdateCmd           , 4},
    {kDeleteCmd           , 3},
    {kMarkProgCmd         , 3},
    {kMarkDoneCmd         , 3},
    {kListCmd             , 2}
};

enum class TaskStatus {
    kUnknown = -1,
    kInProgress,
    kDone
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

static inline std::string GetCurrentTime(const char* fmt = "%Y-%m-%d %T") {
    SystemTimePoint now = SystemClock::now();
    std::ostringstream oss;
    std::time_t now_t = SystemClock::to_time_t(now);
    oss << std::put_time(localtime(&now_t), fmt);
    return oss.str();
}

#endif // TASK_HPP