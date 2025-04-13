#include "task_handler.hpp"

TaskHandler::TaskHandler() {

}

TaskHandler::~TaskHandler() {

}

int TaskHandler::Handle(const std::string& cmd, const std::vector<std::string>& args) {
    if (cmd == kAddCmd) {
        ErrIf(args.size() < 1, "Missing required arguments.");
        return handleAddTask(args[0]);
    } else if (cmd == kUpdateCmd) {
        ErrIf(args.size() < 2, "Missing required arguments.");
        return handleUpdateTask(args);
    } else if (cmd == kDeleteCmd) {
        ErrIf(args.size() < 1, "Missing required arguments.");
        return handleDeleteTask(args[0]);
    } else if (cmd == kMarkProgCmd) {
        ErrIf(args.size() < 1, "Missing required arguments.");
        return handleMarkTask(args[0], TaskStatus::kInProgress);
    } else if (cmd == kMarkDoneCmd) {
        ErrIf(args.size() < 1, "Missing required arguments.");
        return handleMarkTask(args[0], TaskStatus::kDone);
    } else if (cmd == kListCmd) {
        ErrIf(args.size() > 2, "Missing required arguments.");
        return handleListTask(args);
    } else {
        fprintf(stderr, "Unknown cmd: [%s].", cmd.c_str());
        return 1;
    }
}

int TaskHandler::handleAddTask(const std::string& args) {
    Task t {
        .description = args,
        .status = static_cast<int>(TaskStatus::kInProgress)
    };
    
}

int TaskHandler::handleUpdateTask(const std::vector<std::string>& args) {
    auto iter = task_cache_.find(args[0]);
    ErrIf(iter == task_cache_.end(), "Not found this task.");
    iter->second.description = args[1];
}

int TaskHandler::handleMarkTask(const std::string& arg, TaskStatus status) {
    auto iter = task_cache_.find(arg);
    ErrIf(iter == task_cache_.end(), "Not found this task.");
    iter->second.status = static_cast<int>(status);
}

int TaskHandler::handleDeleteTask(const std::string& arg) {
    ErrIf(task_cache_.find(arg) == task_cache_.end(), "Not found this task.");
}

int TaskHandler::handleListTask(const std::vector<std::string>& args) {

}
