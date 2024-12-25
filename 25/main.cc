#include "../common.hh"
#include <array>
#include <cstddef>
#include <cstdio>
#include <vector>

constexpr Vec2<size_t> schematic_size{5, 7};

struct Input {
    std::vector<std::array<size_t, 5>> locks, keys;

    Input(FILE* input) {
        for (;;) {
            Grid<bool> schematic(input, [](char c, auto pos) { return c == '#'; });
            if (schematic.size.is_zero()) {
                break;
            }
            assert(schematic.size == schematic_size);

            bool is_lock = schematic[{0, 0}];

            std::array<size_t, 5> heights;
            for (size_t x = 0; x < schematic.size.x; x++) {
                size_t height = 0;
                if (is_lock) {
                    for (size_t y = 1;
                         y < schematic.size.y && schematic[{x, y}]; y++)
                        height++;
                } else {
                    for (size_t y = schematic.size.y - 1; y-- && schematic[{x, y}];)
                        height++;
                }
                heights[x] = height;
            }

            if (is_lock) {
                locks.push_back(heights);
            } else {
                keys.push_back(heights);
            }
        }
    }
};

void part1(FILE* input_) {
    Input input(input_);
    size_t count = 0;
    for (const auto & lock : input.locks) {
        for (const auto & key : input.keys) {
            for (size_t i = 0; i < schematic_size.x; i++) {
                if (lock[i] + key[i] > schematic_size.y - 2) {
                    goto next;
                }
            }
            count++;
            next:;
        }
    }
    printf("%zu\n", count);
}

void part2(FILE* input_) {
    printf("Merry Christmas!\n");
}
