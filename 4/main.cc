#include "../common.hh"
#include <cassert>
#include <cstring>
#include <string>
#include <vector>

std::vector<char> parse_grid(FILE* input_, size_t& width, size_t& height) {
    std::string input = read_all(input_);
    std::vector<char> grid;
    width = 0;
    height = 0;
    size_t cur_width = 0;
    for (char c : input) {
        if (c == '\n') {
            if (width) {
                assert(cur_width == width);
            } else {
                width = cur_width;
            }
            cur_width = 0;
            height++;
        } else {
            grid.push_back(c);
            cur_width++;
        }
    }
    return grid;
}

void part1(FILE* input) {
    size_t width, height;
    std::vector<char> grid = parse_grid(input, width, height);

    const char* word = "XMAS";
    size_t word_len = strlen(word);

    int dirs_x[] = {0, 1, 1, 1, 0, -1, -1, -1};
    int dirs_y[] = {-1, -1, 0, 1, 1, 1, 0, -1};
    size_t dirs = sizeof(dirs_x) / sizeof(dirs_x[0]);

    size_t count = 0;
    for (ptrdiff_t y = 0; y < height; y++) {
        for (ptrdiff_t x = 0; x < width; x++) {
            if (grid[y * width + x] != word[0])
                continue;
            for (size_t dir = 0; dir < dirs; dir++) {
                ptrdiff_t dx = dirs_x[dir], dy = dirs_y[dir];
                ptrdiff_t end_x = x + dx * (word_len - 1),
                          end_y = y + dy * (word_len - 1);
                if (end_x < 0 || end_x >= width || end_y < 0 || end_y >= height)
                    continue;
                for (size_t i = 1; i < word_len; i++) {
                    if (grid[(y + dy * i) * width + (x + dx * i)] != word[i]) {
                        goto failed;
                    }
                }
                count++;
            failed:;
            }
        }
    }
    printf("%zu\n", count);
}

void part2(FILE* input_) {
    size_t width, height;
    std::vector<char> grid = parse_grid(input_, width, height);

    const char* word = "MAS";
    size_t center_i = 1;

    int dirs_x[] = {1, 1, -1, -1};
    int dirs_y[] = {-1, 1, 1, -1};
    size_t dirs = sizeof(dirs_x) / sizeof(dirs_x[0]);

    size_t count = 0;
    for (ptrdiff_t y = center_i; y < height - center_i; y++) {
        for (ptrdiff_t x = center_i; x < width - center_i; x++) {
            if (grid[y * width + x] != word[center_i])
                continue;
            size_t matched = 0;
            for (size_t dir = 0; dir < dirs && matched < 2; dir++) {
                ptrdiff_t dx = dirs_x[dir], dy = dirs_y[dir];
                for (ptrdiff_t i = 1; i <= center_i; i++) {
                    if (grid[(y + dy * i) * width + (x + dx * i)] !=
                            word[center_i - i] ||
                        grid[(y - dy * i) * width + (x - dx * i)] !=
                            word[center_i + i]) {
                        goto failed;
                    }
                }
                matched++;
            failed:;
            }
            if (matched == 2) {
                count++;
            }
        }
    }
    printf("%zu\n", count);
}
