#pragma once

#include <string>
#include <system_error>

constexpr size_t BUF_START_SIZE = 512;

inline std::string read_all(FILE* f) {
    size_t buf_size = BUF_START_SIZE;
    char* buf = (char*)malloc(buf_size);
    for (size_t i = 0;;) {
        if (i == buf_size) {
            buf_size <<= 1;
            buf = (char*)realloc(buf, buf_size);
        }
        size_t read_bytes = fread(buf + i, 1, buf_size - i, f);
        i += read_bytes;
        int err = ferror(f);
        if (err) {
            throw std::system_error(err, std::system_category());
        }
        if (feof(f))
            break;
    }
    std::string result(buf);
    free(buf);
    return result;
}
