#include "task_handler.hpp"
#include "hjson.hpp"
#include <fstream>

static std::unordered_map<std::string, TaskStatus> support_list_cmds = {
    {"done", TaskStatus::kDone},
    {"todo", TaskStatus::kTodo},
    {"in-progress", TaskStatus::kInProgress}
};

TaskHandler::TaskHandler(): latest_id_(1), updated_(false) {
    init();
}

TaskHandler::~TaskHandler() {
    if (updated_)
    {
        flush();   
    }
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

void TaskHandler::init() {
    HJson* root_node = 0;
    std::ifstream in(kTaskDataBaseName);
    std::ostringstream oss;
    oss << in.rdbuf();
    std::string task_content = oss.str();
    root_node = HJson_parse(task_content.c_str());
    HJson* ptr = root_node->child;
    while (ptr) {
        Task t{};
        HJson* p = ptr->child;
        while (p) {
            std::string key = p->key;
            if (key == "id") {
                t.id = p->sv;
            } else if (key == "description") {
                t.description = p->sv;
            } else if (key == "status") {
                t.status = p->biv;
            } else if (key == "created_at") {
                t.created_at = p->sv;
            } else if (key == "updated_at") {
                t.updated_at = p->sv;
            }
            p = p->next;
        }
        task_cache_[t.id] = t;
        task_all_.push_back(t);
        TaskStatus status = static_cast<TaskStatus>(t.status);
        if (status == TaskStatus::kTodo) {
            task_todo_.push_back(t);
        } else if (status == TaskStatus::kInProgress) {
            task_progress_.push_back(t);
        } else if (status == TaskStatus::kDone) {
            task_done_.push_back(t);
        }
        latest_id_++;
        ptr = ptr->next;
    }
    in.close();
    HJson_delete(root_node);
}

void TaskHandler::flush() {
    HJson* array_node = HJson_createArray();
    for (auto iter = task_cache_.begin(); iter != task_cache_.end(); ++iter) {
        Task& t = iter->second;
        HJson* object_node = HJson_createObject();
        HJson* id_node = HJson_createString(t.id.c_str());
        HJson* description_node = HJson_createString(t.description.c_str());
        HJson* status_node = HJson_createNumber(t.status);
        HJson* created_node = HJson_createString(t.created_at.c_str());
        HJson* updated_node = HJson_createString(t.updated_at.c_str());
        HJson_addItemToObject(object_node, "id", id_node);
        HJson_addItemToObject(object_node, "description", description_node);
        HJson_addItemToObject(object_node, "status", status_node);
        HJson_addItemToObject(object_node, "created_at", created_node);
        HJson_addItemToObject(object_node, "updated_at", updated_node);
        HJson_addItem(array_node, object_node);
    }
    int out_len = 0;
    const char* ret = HJson_write(array_node, out_len);
#ifdef _DEBUG
    std::cout
        << "Flush content: "
        << '\n'
        << ret
        << std::endl;
#endif // _DEBUG
    // Write to json file
    std::ofstream of(kTaskDataBaseName, std::ios::out);
    of.write(ret, out_len);
    of.close();
    delete ret;
    HJson_delete(array_node);
}

int TaskHandler::handleAddTask(const std::string& args) {
    std::string id = std::to_string(latest_id_);
    Task t {
        .id = id,
        .description = args,
        .status = static_cast<int>(TaskStatus::kTodo),
        .created_at = GetCurrentTime(),
        .updated_at = GetCurrentTime()
    };
    task_cache_[id] = t;
    updated_ = true;
    return 0;
}

int TaskHandler::handleUpdateTask(const std::vector<std::string>& args) {
    auto iter = task_cache_.find(args[0]);
    ErrIf(iter == task_cache_.end(), "Not found this task.");
    iter->second.description = args[1];
    iter->second.updated_at = GetCurrentTime();
    updated_ = true;
    return 0;
}

int TaskHandler::handleMarkTask(const std::string& arg, TaskStatus status) {
    auto iter = task_cache_.find(arg);
    ErrIf(iter == task_cache_.end(), "Not found this task.");
    iter->second.status = static_cast<int>(status);
    iter->second.updated_at = GetCurrentTime();
    updated_ = true;
    return 0;
}

int TaskHandler::handleDeleteTask(const std::string& arg) {
    ErrIf(task_cache_.find(arg) == task_cache_.end(), "Not found this task.");
    task_cache_.erase(arg);
    updated_ = true;
    return 0;
}

int TaskHandler::handleListTask(const std::vector<std::string>& args) {
    if (args.empty()) {
        // List all task
        printTask(TaskStatus::kUnknown);
    } else {
        std::string sub_cmd = args[0];
        printTask(support_list_cmds[sub_cmd]);
    }
    return 0;
}

// +------+-------------+--------+--------------+--------------+
// |  id  | description | status | created_time | updated_time |
// +------+-------------+--------+--------------+--------------+
void TaskHandler::printTask(TaskStatus status) {
    char buffer[BUFFER_SIZE] = {0};
    // Table head
    printf("+------+-------------+---------+-------------------+-------------------+\n");
    printf("|  id  | description |  status |    created_time   |    updated_time   |\n");
    printf("+------+-------------+---------+-------------------+-------------------+\n");
    if (status == TaskStatus::kDone) {
        for (auto iter = task_done_.begin(); iter != task_done_.end(); ++iter) {
            snprintf(buffer, BUFFER_SIZE, "%04d", std::stoi(iter->id));
            printf("| %s |", buffer);
            printf("    %s    |", iter->description.c_str());
            printf("    %d    |", iter->status);
            printf("%s|", iter->created_at.c_str());
            printf("%s|\n", iter->updated_at.c_str());
            printf("+------+-------------+---------+-------------------+-------------------+\n");
        }
    } else if (status == TaskStatus::kInProgress) {
        for (auto iter = task_progress_.begin(); iter != task_progress_.end(); ++iter) {
            snprintf(buffer, BUFFER_SIZE, "%04d", std::stoi(iter->id));
            printf("| %s |", buffer);
            printf("    %s    |", iter->description.c_str());
            printf("    %d    |", iter->status);
            printf("%s|", iter->created_at.c_str());
            printf("%s|\n", iter->updated_at.c_str());
            printf("+------+-------------+---------+-------------------+-------------------+\n");
        }
    } else if (status == TaskStatus::kTodo) {
        for (auto iter = task_todo_.begin(); iter != task_todo_.end(); ++iter) {
            snprintf(buffer, BUFFER_SIZE, "%04d", std::stoi(iter->id));
            printf("| %s |", buffer);
            printf("    %s    |", iter->description.c_str());
            printf("    %d    |", iter->status);
            printf("%s|", iter->created_at.c_str());
            printf("%s|\n", iter->updated_at.c_str());
            printf("+------+-------------+---------+-------------------+-------------------+\n");
        }
    } else {
        for (auto iter = task_all_.begin(); iter != task_all_.end(); ++iter) {
            snprintf(buffer, BUFFER_SIZE, "%04d", std::stoi(iter->id));
            printf("| %s |", buffer);
            printf("    %s    |", iter->description.c_str());
            printf("    %d    |", iter->status);
            printf("%s|", iter->created_at.c_str());
            printf("%s|\n", iter->updated_at.c_str());
            printf("+------+-------------+---------+-------------------+-------------------+\n");
        }
    }
}