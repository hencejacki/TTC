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
    int Handle(std::string cmd, std::vector<std::string>& /*args*/);

private:

    int handleAddTask();

    int handleUpdateTask();

    int handleDeleteTask();

    int handleListTask();
};

#endif // TASK_HANDLER_HPP