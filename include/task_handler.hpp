#ifndef TASK_HANDLER_HPP
#define TASK_HANDLER_HPP

#include "helper.hpp"

class TaskHandler {
public:
    TaskHandler();
    virtual ~TaskHandler();

    /* @brief Handle task
     * @param cmd
     * @param args
     * @return Result of handing task
     */
    int Handle(const std::string& /*cmd*/, const std::vector<std::string>& /*args*/);

private:

    void init();

    void flush();
    
    int handleAddTask(const std::string& /*arg*/);

    int handleUpdateTask(const std::vector<std::string>& /*args*/);

    int handleDeleteTask(const std::string& /*arg*/);

    int handleMarkTask(const std::string& /*arg*/, TaskStatus /*status*/);

    int handleListTask(const std::vector<std::string>& /*args*/);

    void printTask(TaskStatus /*status*/);

private:
    std::map<std::string, Task> task_cache_;
    std::vector<Task> task_all_;
    std::vector<Task> task_todo_;
    std::vector<Task> task_progress_;
    std::vector<Task> task_done_;
    int latest_id_;
    bool updated_;
};

#endif // TASK_HANDLER_HPP