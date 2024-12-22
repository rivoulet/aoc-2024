#include "../common.hh"
#include <cassert>
#include <cstring>
#include <vector>

std::pair<std::vector<char>, Vec2<size_t>> parse_grid(FILE* input) {
    std::vector<char> grid;
    Vec2<size_t> size{0, 0};
    size_t cur_width = 0;
    for (int c; (c = read_char(input)) != EOF;) {
        if (c == '\n') {
            if (size.x) {
                assert(cur_width == size.x);
            } else {
                size.x = cur_width;
            }
            cur_width = 0;
            size.y++;
        } else {
            grid.push_back(c);
            cur_width++;
        }
    }
    return {grid, size};
}

void part1(FILE* input) {
    auto [grid, grid_size] = parse_grid(input);

    const char* word = "XMAS";
    size_t word_len = strlen(word);

    int dirs_x[] = {0, 1, 1, 1, 0, -1, -1, -1};
    int dirs_y[] = {-1, -1, 0, 1, 1, 1, 0, -1};
    size_t dirs = sizeof(dirs_x) / sizeof(dirs_x[0]);

    size_t count = 0;
    for (ptrdiff_t y = 0; y < grid_size.y; y++) {
        for (ptrdiff_t x = 0; x < grid_size.x; x++) {
            if (grid[y * grid_size.x + x] != word[0])
                continue;
            for (size_t dir = 0; dir < dirs; dir++) {
                ptrdiff_t dx = dirs_x[dir], dy = dirs_y[dir];
                ptrdiff_t end_x = x + dx * (word_len - 1),
                          end_y = y + dy * (word_len - 1);
                if (end_x < 0 || end_x >= grid_size.x || end_y < 0 ||
                    end_y >= grid_size.y)
                    continue;
                for (size_t i = 1; i < word_len; i++) {
                    if (grid[(y + dy * i) * grid_size.x + (x + dx * i)] !=
                        word[i]) {
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

void part2(FILE* input) {
    auto [grid, grid_size] = parse_grid(input);

    const char* word = "MAS";
    size_t center_i = 1;

    int dirs_x[] = {1, 1, -1, -1};
    int dirs_y[] = {-1, 1, 1, -1};
    size_t dirs = sizeof(dirs_x) / sizeof(dirs_x[0]);

    size_t count = 0;
    for (ptrdiff_t y = center_i; y < grid_size.y - center_i; y++) {
        for (ptrdiff_t x = center_i; x < grid_size.x - center_i; x++) {
            if (grid[y * grid_size.x + x] != word[center_i])
                continue;
            size_t matched = 0;
            for (size_t dir = 0; dir < dirs && matched < 2; dir++) {
                ptrdiff_t dx = dirs_x[dir], dy = dirs_y[dir];
                for (ptrdiff_t i = 1; i <= center_i; i++) {
                    if (grid[(y + dy * i) * grid_size.x + (x + dx * i)] !=
                            word[center_i - i] ||
                        grid[(y - dy * i) * grid_size.x + (x - dx * i)] !=
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
