#include <cstddef>
#include <cstdio>
#include <vector>

const size_t LINE_BUF_SIZE = 512;

bool are_valid(const std::vector<unsigned>& levels, size_t skip_i) {
    unsigned prev_level = 0;
    bool is_increasing = false;
    for (size_t i = 0, j = 0; i < levels.size(); i++) {
        unsigned level = levels[i];
        if (i == skip_i)
            continue;
        if (j) {
            unsigned diff =
                level > prev_level ? level - prev_level : prev_level - level;
            if (diff < 1 || diff > 3) {
                return false;
            }
            if (j == 1) {
                is_increasing = level > prev_level;
            } else {
                if ((level > prev_level) != is_increasing) {
                    return false;
                }
            }
        }
        prev_level = level;
        j++;
    }
    return true;
}

void part1(FILE* input) {
    char line[LINE_BUF_SIZE];
    size_t valid_count = 0;
    while (fgets(line, LINE_BUF_SIZE, input)) {
        std::vector<unsigned> levels;
        for (const char* cur = line;;) {
            unsigned level;
            unsigned read_chars;
            if (sscanf(cur, "%d%n", &level, &read_chars) <= 0)
                break;
            cur += read_chars;
            levels.push_back(level);
        }

        const char* cur = line;
        unsigned prev_level = 0;
        bool is_increasing = false;
        bool is_valid = true;
        for (size_t i = 0; is_valid; i++) {
            unsigned level;
            unsigned read_chars;
            if (sscanf(cur, "%d%n", &level, &read_chars) <= 0)
                break;
            cur += read_chars;
            if (i) {
                unsigned diff = level > prev_level ? level - prev_level
                                                   : prev_level - level;
                if (diff < 1 || diff > 3) {
                    is_valid = false;
                    break;
                }
                if (i == 1) {
                    is_increasing = level > prev_level;
                } else {
                    if ((level > prev_level) != is_increasing) {
                        is_valid = false;
                        break;
                    }
                }
            }
            prev_level = level;
        }
        valid_count += is_valid;
    }
    printf("%zu\n", valid_count);
}

void part2(FILE* input) {
    char line[LINE_BUF_SIZE];
    size_t valid_count = 0;
    while (fgets(line, LINE_BUF_SIZE, input)) {
        std::vector<unsigned> levels;
        for (const char* cur = line;;) {
            unsigned level;
            unsigned read_chars;
            if (sscanf(cur, "%d%n", &level, &read_chars) <= 0)
                break;
            cur += read_chars;
            levels.push_back(level);
        }
        bool is_valid = false;
        for (size_t i = 0; i < levels.size() && !is_valid; i++) {
            is_valid = are_valid(levels, i);
        }
        valid_count += is_valid;
    }
    printf("%zu\n", valid_count);
}
