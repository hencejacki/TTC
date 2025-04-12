#include "task_handler.hpp"

int main(int argc, char const *argv[])
{
    std::string prog_name = argv[0];
    if (argc < 2) {
        ShowUsage(prog_name);
        return 1;
    }
    std::string cmd = argv[1];
    auto iter = support_cmd.find(cmd);
    ErrIf(iter == support_cmd.end(), "Unsupport command: [%s].", cmd);
    // Check argc
    ErrIf(argc < iter->second, "Unexpected argument count, expected: %d, got: %d.", iter->second, argc);

    return 0;
}
