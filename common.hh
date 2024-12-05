#pragma once

#include <cstdio>
#include <string>
#include <system_error>
#include <vector>

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

inline std::vector<std::string> read_all_lines(FILE* f) {
    std::vector<std::string> result;
    size_t buf_size = BUF_START_SIZE;
    char* buf = (char*)malloc(buf_size);
    size_t i = 0;
    do {
        if (i == buf_size) {
            buf_size <<= 1;
            buf = (char*)realloc(buf, buf_size);
        }
        if (fgets(buf + i, buf_size - i, f)) {
            i += strlen(buf + i);
            if (!feof(f) && buf[i - 1] != '\n')
                continue;
        } else {
            int err = ferror(f);
            if (err) {
                throw std::system_error(err, std::system_category());
            }
            // EOF
            buf[i] = 0;
        }
        result.push_back(buf);
        i = 0;
    } while (!feof(f));
    free(buf);
    return result;
}
