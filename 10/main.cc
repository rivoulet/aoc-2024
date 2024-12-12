#include "../common.hh"
#include <algorithm>
#include <cstddef>
#include <vector>

struct InputGrid : Grid<uint8_t> {
    std::vector<Vec2<size_t>> zeros;
    std::vector<Vec2<size_t>> nines;

    InputGrid(FILE* input) : Grid(input, [](char c) { return c - '0'; }) {
        for (size_t y = 0; y < size.y; y++) {
            for (size_t x = 0; x < size.x; x++) {
                switch (contents[pos_to_i({x, y})]) {
                case 0: {
                    zeros.push_back({x, y});
                    break;
                }
                case 9: {
                    nines.push_back({x, y});
                    break;
                }
                }
            }
        }
    }
};

void part1(FILE* input) {
    InputGrid grid(input);

    size_t sum = 0;

    std::vector<Vec2<size_t>> to_visit;
    std::vector<bool> visited(grid.contents.size(), false);
    for (const auto zero : grid.zeros) {
        std::fill(visited.begin(), visited.end(), false);
        to_visit.push_back(zero);
        visited[grid.pos_to_i(zero)] = true;
        while (!to_visit.empty()) {
            auto pos = to_visit.back();
            to_visit.pop_back();
            auto i = grid.pos_to_i(pos);
            for (const auto dir : grid_dirs) {
                auto new_pos = pos + dir;
                if (new_pos.x >= grid.size.x || new_pos.y >= grid.size.y)
                    continue;
                auto j = grid.pos_to_i(new_pos);
                if (visited[j] || grid.contents[j] != grid.contents[i] + 1)
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
    InputGrid grid(input);

    size_t sum = 0;

    std::vector<Vec2<size_t>> to_visit;
    for (const auto nine : grid.nines) {
        to_visit.push_back(nine);
        while (!to_visit.empty()) {
            auto pos = to_visit.back();
            to_visit.pop_back();
            auto i = grid.pos_to_i(pos);
            sum += grid.contents[i] == 0;
            for (const auto dir : grid_dirs) {
                auto new_pos = pos + dir;
                if (new_pos.x >= grid.size.x || new_pos.y >= grid.size.y)
                    continue;
                auto j = grid.pos_to_i(new_pos);
                if (grid.contents[j] + 1 != grid.contents[i])
                    continue;
                to_visit.push_back(new_pos);
            }
        }
    }

    printf("%zu\n", sum);
}
