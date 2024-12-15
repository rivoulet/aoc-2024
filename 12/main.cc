#include "../common.hh"
#include <cstddef>
#include <cstdint>
#include <queue>
#include <vector>

constexpr std::array<std::array<Vec2<ptrdiff_t>, 2>, 4> perp_grid_dir_offsets{
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
    Grid<char> grid(input);
    Grid<bool> visited(grid.size);
    std::vector<Vec2<size_t>> to_visit;

    size_t sum = 0;
    for (size_t y = 0; y < grid.size.y; y++) {
        for (size_t x = 0; x < grid.size.x; x++) {
            char region_value;
            {
                Vec2<size_t> pos = {x, y};
                if (visited[pos])
                    continue;
                visited[pos] = true;
                to_visit.push_back(pos);
                region_value = grid[pos];
            }
            size_t area = 0, perimeter = 0;
            while (!to_visit.empty()) {
                auto pos = to_visit.back();
                to_visit.pop_back();
                area++;
                for (const auto dir_offset : grid_dir_offsets) {
                    auto other_pos = pos + dir_offset;
                    if (other_pos.x >= grid.size.x ||
                        other_pos.y >= grid.size.y ||
                        grid[other_pos] != region_value) {
                        perimeter++;
                        continue;
                    }
                    if (visited[other_pos])
                        continue;
                    visited[other_pos] = true;
                    to_visit.push_back(other_pos);
                }
            }
            sum += area * perimeter;
        }
    }

    printf("%zu\n", sum);
}

void part2(FILE* input) {
    Grid<char> grid(input);
    Grid<bool> visited(grid.size, false);
    Grid<uint8_t> side_masks(grid.size, 0);
    std::queue<Vec2<size_t>> to_visit;

    size_t sum = 0;
    for (size_t y = 0; y < grid.size.y; y++) {
        for (size_t x = 0; x < grid.size.x; x++) {
            char region_value;
            {
                Vec2<size_t> pos = {x, y};
                if (visited[pos])
                    continue;
                visited[pos] = true;
                to_visit.push(pos);
                region_value = grid[pos];
            }
            size_t area = 0, sides = 0;
            while (!to_visit.empty()) {
                auto pos = to_visit.front();
                to_visit.pop();
                area++;
                for (size_t dir_i = 0; dir_i < grid_dir_offsets.size(); dir_i++) {
                    auto other_pos = pos + grid_dir_offsets[dir_i];
                    if (other_pos.x >= grid.size.x ||
                        other_pos.y >= grid.size.y ||
                        grid[other_pos] != region_value) {
                        auto side_mask = 1 << dir_i;
                        side_masks[pos] |= side_mask;
                        for (const auto perp_dir : perp_grid_dir_offsets[dir_i]) {
                            auto adj_pos = pos + perp_dir;
                            if (adj_pos.x >= grid.size.x ||
                                adj_pos.y >= grid.size.y ||
                                grid[adj_pos] != region_value)
                                continue;
                            if (side_masks[adj_pos] & side_mask)
                                goto found_adjacent;
                        }
                        sides++;
                    found_adjacent:
                        continue;
                    }
                    if (visited[other_pos])
                        continue;
                    visited[other_pos] = true;
                    to_visit.push(other_pos);
                }
            }
            sum += area * sides;
        }
    }

    printf("%zu\n", sum);
}
