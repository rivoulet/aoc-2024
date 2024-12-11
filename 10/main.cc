#include "../common.hh"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <vector>

struct Grid {
    std::vector<uint8_t> heights;
    Vec2<size_t> size;
    std::vector<Vec2<size_t>> zeros;
    std::vector<Vec2<size_t>> nines;

    static Grid parse(FILE* input) {
        Grid grid{{}, {0, 0}};
        size_t cur_width = 0;
        for (int c; (c = read_char(input)) != EOF;) {
            if (c == '\n') {
                if (grid.size.x) {
                    assert(cur_width == grid.size.x);
                } else {
                    grid.size.x = cur_width;
                }
                cur_width = 0;
                grid.size.y++;
            } else {
                grid.heights.push_back(c - '0');
                cur_width++;
            }
        }

        for (size_t y = 0; y < grid.size.y; y++) {
            for (size_t x = 0; x < grid.size.x; x++) {
                switch (grid.heights[grid.pos_to_i({x, y})]) {
                case 0: {
                    grid.zeros.push_back({x, y});
                    break;
                }
                case 9: {
                    grid.nines.push_back({x, y});
                    break;
                }
                }
            }
        }

        return grid;
    }

    size_t pos_to_i(Vec2<size_t> pos) {
        return pos.y * size.x + pos.x;
    }
};

constexpr std::array<Vec2<ptrdiff_t>, 4> dirs{
    Vec2<ptrdiff_t>{1, 0},
    {-1, 0},
    {0, 1},
    {0, -1},
};

void part1(FILE* input) {
    auto grid = Grid::parse(input);

    size_t sum = 0;

    std::vector<Vec2<size_t>> to_visit;
    std::vector<bool> visited(grid.heights.size(), false);
    for (const auto zero : grid.zeros) {
        std::fill(visited.begin(), visited.end(), false);
        to_visit.push_back(zero);
        visited[grid.pos_to_i(zero)] = true;
        while (!to_visit.empty()) {
            auto pos = to_visit.back();
            to_visit.pop_back();
            auto i = grid.pos_to_i(pos);
            for (const auto dir : dirs) {
                auto new_pos = pos + dir;
                if (new_pos.x >= grid.size.x || new_pos.y >= grid.size.y)
                    continue;
                auto j = grid.pos_to_i(new_pos);
                if (visited[j] || grid.heights[j] != grid.heights[i] + 1)
                    continue;
                visited[j] = true;
                to_visit.push_back(new_pos);
            }
        }

        for (const auto nine : grid.nines) {
            sum += visited[grid.pos_to_i(nine)];
        }
    }

    printf("%zu\n", sum);
}

void part2(FILE* input) {
    auto grid = Grid::parse(input);

    size_t sum = 0;

    std::vector<Vec2<size_t>> to_visit;
    for (const auto nine : grid.nines) {
        to_visit.push_back(nine);
        while (!to_visit.empty()) {
            auto pos = to_visit.back();
            to_visit.pop_back();
            auto i = grid.pos_to_i(pos);
            sum += grid.heights[i] == 0;
            for (const auto dir : dirs) {
                auto new_pos = pos + dir;
                if (new_pos.x >= grid.size.x || new_pos.y >= grid.size.y)
                    continue;
                auto j = grid.pos_to_i(new_pos);
                if (grid.heights[j] + 1 != grid.heights[i])
                    continue;
                to_visit.push_back(new_pos);
            }
        }
    }

    printf("%zu\n", sum);
}
