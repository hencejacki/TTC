#include "hjson.hpp"
#include <fstream>
#include <iostream>

HJson* TestDeserialize(const char* json_file) {
    // Reading file
    std::ifstream in(json_file);
    char f_buf[1024] = {0};
    in.read(f_buf, 1024);
    std::cout
        << f_buf
        << std::endl;
    // Deserialize
    return HJson_parse(f_buf);
}

void TestSerialize(HJson* node) {
    // Serialize
    int out_len = 0;
    const char* ret = HJson_write(node, out_len);
    std::cout
        << "Serialize result: "
        << '\n'
        << ret
        << std::endl;
    delete ret;
}

void TestCreateArray() {
    HJson* array_node = HJson_createArray();
    HJson* object_node = HJson_createObject();
    HJson* string_node = HJson_createString("Value-1");
    HJson_addItemToObject(object_node, "Key-1", string_node);
    HJson* string_node1 = HJson_createString("Value-2");
    HJson_addItemToObject(object_node, "Key-2", string_node1);
    HJson_addItem(array_node, object_node);
    TestSerialize(array_node);
    HJson_delete(array_node);
}

int main(int argc, char const *argv[])
{
    HJson* root_node = 0;
    root_node = TestDeserialize("task.json");
    TestSerialize(root_node);
    TestCreateArray();
    HJson_delete(root_node);
    return 0;
}
