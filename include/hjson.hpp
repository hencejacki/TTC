#ifndef HJSON_HPP
#define HJSON_HPP

#include "err.hpp"
#include <cstring>
#include <string>

/* TODO:
 * [2025-04-13]
 * 1. ParseArray assign value.
 * 2. ParseBoolean check.
 * 3. JsonParse separator handle.
 * 4. JsonParse test.
 * 5. JsonWrite implement.
 */

#define BUFFER_SIZE 32

struct HJson {
    enum Type { OBJECT, EMPTY, ARRAY, BOOLEAN, DOUBLE, INTEGER, STRING};

    union Value
    {
        HJson*       ov;   // object-null
        Value*       av;   // array
        bool         bv;   // true-false
        int          iv;   // number
        double       dv;   // number
        char*        sv;   // string
        
        Type t;

        HJson* ToObj() {
            if (t == OBJECT) {
                return ov;     
            }
            return nullptr;
        }

        template<typename T>
        T* ToArr() {
            if (t == ARRAY) {
                return static_cast<T*>(av);
            }
            return nullptr;
        }

        bool ToBool() {
            ErrIf(t != BOOLEAN, "Not bool type, maybe.");
            return bv;
        }

        int ToInt() {
            ErrIf(t != INTEGER, "Not int type.");
            return iv;
        }

        double ToDouble() {
            ErrIf(t != DOUBLE, "Not double type");
            return dv;
        }

        const char* ToString() {
            ErrIf(t != STRING, "Not string type");
            return sv;
        }
    }v;

    char k[BUFFER_SIZE];

    Value operator[](const char* key) {
        ErrIf(k != key, "Unrecognized key, maybe [%s]?", k);
        return v;
    }

    ~HJson() {
        if (v.t == OBJECT) {
            v.ov->~HJson();       
        } else if (v.t == ARRAY) {
            delete[] v.av;
        } else if (v.t == STRING) {
            delete[] v.sv;
        }
    }
};

static inline void ParseArray(HJson::Value* v, const char* content, int& offset) {
    const char* p = content;
    while (*p != '\0' && (*p == ' ' || *p == '\r' || *p == '\n')) p++;
    ErrIf(*p == ',', "Unrecognized format.");
    char s_buffer[BUFFER_SIZE][BUFFER_SIZE] = {0};
    int s_cursor = 0;
    int i_buffer[BUFFER_SIZE] = {0};
    int i_cursor = 0;
    int d_buffer[BUFFER_SIZE] = {0};
    int d_cursor = 0;
    bool b_buffer[BUFFER_SIZE] = {0};
    int b_cursor = 0;
    HJson o_buffer[BUFFER_SIZE] = {0};
    int o_cursor = 0;
    HJson::Value v_buffer[BUFFER_SIZE] = {0};
    int v_cursor = 0;
    while (*p != '\0' && *p != ']') {
        while (*p != '\0' && (*p == ' ' || *p == '\r' || *p == '\n')) p++;
        if (*p == '{') {
            // Object
            HJson node;
            int off = 0;
            JsonParse(&node, ++p, off);
            o_buffer[o_cursor++] = node;
            p += off;
        } else if (*p == '[') {
            // Array
            HJson::Value value;
            int off = 0;
            ParseArray(&value, ++p, off);
            v_buffer[v_cursor++] = value;
            p += off;
        } else if (*p == '\"') {
            // String
            char* buffer = nullptr;
            int off = 0;
            ParseString(buffer, ++p, off);
            memcpy(s_buffer[s_cursor++], buffer, BUFFER_SIZE);
            p += off;
            delete[] buffer;
        } else if (*(p + 4) != '\0' && (*p == 't' || *p == 'f')) {
            // Boolean
            bool value = false;
            int off = 0;
            ParseBoolean(value, ++p, off);
            b_buffer[b_cursor++] = value;
            p += off;
        } else if (*p >= '0' && *p <= '9') {
            // Number
            HJson::Value value;
            int off = 0;
            ParseNumber(value, ++p, off);
            if (value.t == HJson::DOUBLE) {
                d_buffer[d_cursor++] = value.dv;
            } else if (value.t == HJson::INTEGER) {
                i_buffer[i_cursor++] = value.iv;
            }
            p += off;
        } else if (*p == ',') {
            // Separator
            p++;
            continue;
        } else {
            ErrIf(true, "Unrecognized format.");
        }
    }
}

static inline void ParseBoolean(bool& v, const char* content, int& offset) {
    offset = 4;
    const char* p = content;
    while (*p != '\0' && *p != ',' && *p != '}' && *p != ']' && *p != ' ' && *p != '\r' && *p != '\n') p++;
    ErrIf((p - content) != 4, "Unrecognized format, expected: [%d], got: [%d].", offset, p - content);
    char buffer[BUFFER_SIZE] = {0};
    memcpy(buffer, content, offset);
    if (buffer == "true") {
        v = true;
    } else if (buffer == "false") {
        v = false;
    } else {
        ErrIf(true, "Unrecognized format, got: [%s].", buffer);
    }
}

static inline void ParseString(char* v, const char* content, int& offset) {
    const char* p = content;
    char buffer[BUFFER_SIZE] = {0};
    int cursor = 0;
    while (*p != '\0' && *p != '\"') {
        if (*p == '}' || *p == ']' || *p == ',') {
            ErrIf(true, "Unexpected ending.");
        }
        buffer[cursor++] = *p;
        p++;
    }
    ErrIf(*p == '\0', "Unexpected ending.");
    char* tmp = new char[cursor + 1];
    memcpy(tmp, buffer, cursor);
    v = tmp;
    tmp = nullptr;
    offset = cursor;
}

static inline void ParseNumber(HJson::Value& v, const char* content, int& offset) {
    const char* p = content;
    bool double_flag = false;
    char buffer[BUFFER_SIZE] = {0};
    int cursor = 0;
    while (*p != '\0' && (*p != ',' || *p != '}' || *p != ']')) {
        if (*p == '.') {
            double_flag = true;
        }
        buffer[cursor++] = *p;
        p++;
    }
    try
    {
        if (double_flag) {
            // Double
            v.t = HJson::DOUBLE;
            v.dv = std::stod(buffer);
        } else {
            // Integer
            v.t = HJson::INTEGER;
            v.iv = std::stoi(buffer);
        }   
        offset = cursor;
    }
    catch(const std::exception& e)
    {
        ErrIf(true, e.what());
    }
}

static inline void JsonParse(HJson* root_node, const char* content, int& offset) {
    const char *p = content;
    while (*p != '\0') {
        while (*p != '\0' && (*p == ' ' || *p == '\r' || *p == '\n')) p++;
        // {
        ErrIf(*p++ != '{', "Illegal format, missing {.");
        while (*p != '\0' && (*p == ' ' || *p == '\r' || *p == '\n')) p++;
        // "
        ErrIf(*p++ != '\"', "Illegal format, missing \".");
        char key[BUFFER_SIZE] = {0};
        int i = 0;
        while (*p != '\0' && *p != '\"') key[i++] = *p++;
        memcpy(root_node->k, key, i);
        // "
        p++;
        while (*p != '\0' && (*p == ' ' || *p == '\r' || *p == '\n')) p++;
        // :
        ErrIf(*p++ != ':', "Illegal format, missing :.");
        while (*p != '\0' && (*p == ' ' || *p == '\r' || *p == '\n')) p++;
        ErrIf(*p == ',', "Illegal format, missing value.");
        // value begin
        if (*p == '{') {
            // Object
            root_node->v.t = HJson::OBJECT;
            root_node->v.ov = new HJson();
            int offset = 0;
            JsonParse(root_node->v.ov, p, offset);
            p += offset;
        } else if (*p == '[') {
            // Array
            root_node->v.t = HJson::ARRAY;
            int offset = 0;
            ParseArray(root_node->v.av, ++p, offset);
            p += offset;
        } else if (*p == '\"') {
            // String
            root_node->v.t = HJson::STRING;
            int offset = 0;
            ParseString(root_node->v.sv, ++p, offset);
            p += offset;
        } else if (*(p + 4) != '\0' && (*p == 't' || *p == 'f')) {
            // Boolean
            root_node->v.t = HJson::BOOLEAN;
            int offset = 0;
            ParseBoolean(root_node->v.bv, p, offset);
            p += offset;
        } else if (*p >= '0' && *p <= '9') {
            // Number
            int offset = 0;
            ParseNumber(root_node->v, p, offset);
            p += offset;
        } else {
            ErrIf(true, "Unrecognized format.");
        }
    }
}
static inline void JsonWrite(HJson& root_node, const char* file_name = "output.json") {

}

#endif // HJSON_HPP
