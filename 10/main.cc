#include "../common.hh"
#include <cstddef>
#include <vector>

struct InputGrid : Grid<uint8_t> {
    std::vector<Vec2<size_t>> zeros;
    std::vector<Vec2<size_t>> nines;

    InputGrid(FILE* input) : Grid(input, [](char c) { return c - '0'; }) {
        for (size_t y = 0; y < size.y; y++) {
            for (size_t x = 0; x < size.x; x++) {
                switch ((*this)[{x, y}]) {
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
    Grid<bool> visited(grid.size);
    for (const auto zero : grid.zeros) {
        visited.fill(false);
        to_visit.push_back(zero);
        visited[zero] = true;
        while (!to_visit.empty()) {
            auto pos = to_visit.back();
            to_visit.pop_back();
            for (const auto dir : grid_dirs) {
                auto new_pos = pos + dir;
                if (new_pos.x >= grid.size.x || new_pos.y >= grid.size.y)
                    continue;
                if (visited[new_pos] || grid[new_pos] != grid[pos] + 1)
                    continue;
                visited[new_pos] = true;
                to_visit.push_back(new_pos);
            }
        }

        for (const auto nine : grid.nines) {
            sum += visited[nine];
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
            sum += grid[pos] == 0;
            for (const auto dir : grid_dirs) {
                auto new_pos = pos + dir;
                if (new_pos.x >= grid.size.x || new_pos.y >= grid.size.y)
                    continue;
                if (grid[new_pos] + 1 != grid[pos])
                    continue;
                to_visit.push_back(new_pos);
            }
        }
    }

    printf("%zu\n", sum);
}
