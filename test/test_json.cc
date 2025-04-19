#include "hjson.hpp"
#include <fstream>
#include <iostream>

int main(int argc, char const *argv[])
{
    HJson* root_node = 0;
    // Reading file
    std::ifstream in("task.json");
    char f_buf[1024] = {0};
    in.read(f_buf, 1024);
    std::cout
        << f_buf
        << std::endl;
    // Parse
    try
    {
        root_node = HJson_parse(f_buf);
        std::cout
            << "Done!"
            << std::endl;
        const char* ret = HJson_write(root_node);
        std::cout
            << "Read Json:"
            << '\n'
            << ret
            << std::endl;
        HJson_delete(root_node);
        delete ret;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}
