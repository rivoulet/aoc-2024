#include "../common.hh"
#include <cstddef>
#include <cstdint>
#include <queue>
#include <vector>

constexpr std::array<std::array<Vec2<ptrdiff_t>, 2>, 4> perp_grid_dirs{
    std::array<Vec2<ptrdiff_t>, 2>{
        Vec2<ptrdiff_t>{0, 1},
        {0, -1},
    },
    {
        Vec2<ptrdiff_t>{0, 1},
        {0, -1},
    },
    {
        Vec2<ptrdiff_t>{1, 0},
        {-1, 0},
    },
    {
        Vec2<ptrdiff_t>{1, 0},
        {-1, 0},
    },
};

void part1(FILE* input) {
    Grid<char> grid(input, [](char c) { return c; });
    std::vector<bool> visited(grid.contents.size(), false);
    std::vector<Vec2<size_t>> to_visit;

    size_t sum = 0;
    for (size_t y = 0; y < grid.size.y; y++) {
        for (size_t x = 0; x < grid.size.x; x++) {
            char region_value;
            {
                Vec2<size_t> pos = {x, y};
                auto i = grid.pos_to_i(pos);
                if (visited[i])
                    continue;
                visited[i] = true;
                to_visit.push_back(pos);
                region_value = grid.contents[i];
            }
            size_t area = 0, perimeter = 0;
            while (!to_visit.empty()) {
                auto pos = to_visit.back();
                to_visit.pop_back();
                area++;
                for (const auto dir : grid_dirs) {
                    auto other_pos = pos + dir;
                    auto other_i = grid.pos_to_i(other_pos);
                    if (other_pos.x >= grid.size.x ||
                        other_pos.y >= grid.size.y ||
                        grid.contents[other_i] != region_value) {
                        perimeter++;
                        continue;
                    }
                    if (visited[other_i])
                        continue;
                    visited[other_i] = true;
                    to_visit.push_back(other_pos);
                }
            }
            sum += area * perimeter;
        }
    }

    printf("%zu\n", sum);
}

void part2(FILE* input) {
    Grid<char> grid(input, [](char c) { return c; });
    std::vector<bool> visited(grid.contents.size(), false);
    std::vector<uint8_t> side_masks(grid.contents.size(), 0);
    std::queue<Vec2<size_t>> to_visit;

    size_t sum = 0;
    for (size_t y = 0; y < grid.size.y; y++) {
        for (size_t x = 0; x < grid.size.x; x++) {
            char region_value;
            {
                Vec2<size_t> pos = {x, y};
                auto i = grid.pos_to_i(pos);
                if (visited[i])
                    continue;
                visited[i] = true;
                to_visit.push(pos);
                region_value = grid.contents[i];
            }
            size_t area = 0, sides = 0;
            while (!to_visit.empty()) {
                auto pos = to_visit.front();
                to_visit.pop();
                auto i = grid.pos_to_i(pos);
                area++;
                for (size_t dir_i = 0; dir_i < grid_dirs.size(); dir_i++) {
                    auto other_pos = pos + grid_dirs[dir_i];
                    auto other_i = grid.pos_to_i(other_pos);
                    if (other_pos.x >= grid.size.x ||
                        other_pos.y >= grid.size.y ||
                        grid.contents[other_i] != region_value) {
                        auto side_mask = 1 << dir_i;
                        side_masks[i] |= side_mask;
                        for (const auto perp_dir : perp_grid_dirs[dir_i]) {
                            auto adj_pos = pos + perp_dir;
                            auto adj_i = grid.pos_to_i(adj_pos);
                            if (adj_pos.x >= grid.size.x ||
                                adj_pos.y >= grid.size.y ||
                                grid.contents[adj_i] != region_value)
                                continue;
                            if (side_masks[adj_i] & side_mask)
                                goto found_adjacent;
                        }
                        sides++;
                    found_adjacent:
                        continue;
                    }
                    if (visited[other_i])
                        continue;
                    visited[other_i] = true;
                    to_visit.push(other_pos);
                }
            }
            sum += area * sides;
        }
    }

    printf("%zu\n", sum);
}
