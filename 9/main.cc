#include "../common.hh"
#include <cassert>
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

// struct File {
//     size_t location;
//     size_t id;
//     size_t length;
// };

// struct DiskMap {
//     std::vector<File> files;
// };

std::vector<std::optional<size_t>> parse_input(FILE* input) {
    // std::pair<std::vector<std::pair<size_t, File>>, size_t> result;
    // bool is_file = true;
    // size_t i = 0;
    // size_t file_id = 0;
    // for (int c; (c = read_char(input)) != EOF;) {
    //     unsigned char length = c - '0';
    //     if (length > 9) break;
    //     if (is_file) {
    //         result.push_back({i, {file_id++, length}});
    //     }
    //     i += length;
    // }
    // return result;
    std::vector<std::optional<size_t>> result;
    bool is_file = true;
    size_t file_id = 0;
    for (int c; (c = read_char(input)) != EOF;) {
        unsigned char length = c - '0';
        if (length > 9)
            break;
        auto contents = is_file ? file_id++ : std::optional<size_t>();
        result.resize(result.size() + length, contents);
        is_file = !is_file;
    }
    assert(result.size() >= 1);
    return result;
}

void part1(FILE* input) {
    auto map = parse_input(input);

    for (size_t i = 0, j = map.size() - 1;;) {
        while (i < j) {
            if (!map[i].has_value())
                break;
            i++;
        }
        while (i < j) {
            if (map[j].has_value())
                break;
            j--;
        }
        if (i >= j)
            break;
        std::swap(map[i], map[j]);
    }

    size_t checksum = 0;
    for (size_t i = 0; i < map.size() && map[i].has_value(); i++) {
        checksum += i * map[i].value();
    }
    printf("%zu\n", checksum);
}

void part2(FILE* input) {
    auto map = parse_input(input);

    for (size_t j_end = map.size() - 1;;) {
        while (j_end > 0) {
            if (map[j_end].has_value())
                break;
            j_end--;
        }
        size_t j_start = j_end;
        while (j_start > 0) {
            if (map[j_start - 1] != map[j_end])
                break;
            j_start--;
        }
        size_t move_len = j_end - j_start + 1;
        size_t i_start = 0, i_end = 0;
        for (;;) {
            while (i_start < j_start) {
                if (!map[i_start].has_value())
                    break;
                i_start++;
            }
            i_end = i_start;
            while (i_end < j_start && (i_end - i_start + 1) < move_len) {
                if (map[i_end + 1].has_value())
                    break;
                i_end++;
            }
            if (i_end >= j_start || (i_end - i_start + 1) >= move_len)
                break;
            i_start = i_end + 1;
        }
        if (i_end < j_start) {
            for (size_t k = 0; k < move_len; k++) {
                std::swap(map[i_start++], map[j_end--]);
            }
        }
        if (j_start == 0)
            break;
        j_end = j_start - 1;
    }

    size_t checksum = 0;
    for (size_t i = 0; i < map.size(); i++) {
        checksum += i * map[i].value_or(0);
    }
    printf("%zu\n", checksum);
}
