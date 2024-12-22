#include "../common.hh"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <queue>
#include <vector>

enum class Cell {
    Empty,
    Wall,
};

struct Input {
    Grid<Cell> grid;
    Vec2<size_t> start_pos, end_pos;

    Input(FILE* input_)
        : grid(input_, [&](char c, Vec2<size_t> pos) {
              switch (c) {
              case '#':
                  return Cell::Wall;
              case 'S': {
                  start_pos = pos;
                  break;
              }
              case 'E': {
                  end_pos = pos;
                  break;
              }
              }
              return Cell::Empty;
          }) {}

    Grid<size_t> get_distances() {
        Grid<size_t> distances(grid.size, SIZE_MAX);
        std::queue<Vec2<size_t>> to_visit;
        auto try_visit = [&](auto pos, auto distance) {
            if (distances[pos] != SIZE_MAX)
                return;
            distances[pos] = distance;
            to_visit.push(pos);
        };
        try_visit(start_pos, 0);
        while (!to_visit.empty()) {
            auto pos = to_visit.front();
            to_visit.pop();
            for (const auto dir : grid_dirs) {
                auto new_pos = pos + grid_dir_offset(dir);
                if (grid.pos_is_inside(new_pos) && grid[new_pos] == Cell::Empty)
                    try_visit(new_pos, distances[pos] + 1);
            }
        }
        return distances;
    }

    size_t count_cheats(size_t min_distance, size_t max_distance,
                        size_t min_saving) {
        std::vector<Vec2<ptrdiff_t>> cheat_end_offsets;
        for (ptrdiff_t dy = -(ptrdiff_t)max_distance;
             dy <= (ptrdiff_t)max_distance; dy++) {
            auto abs_y = llabs(dy);
            for (ptrdiff_t dx = -(ptrdiff_t)max_distance + abs_y;
                 dx <= (ptrdiff_t)max_distance - abs_y; dx++) {
                if (llabs(dx) + abs_y < min_distance)
                    continue;
                cheat_end_offsets.push_back({dx, dy});
            }
        }

        const auto distances = get_distances();

        size_t count = 0;
        for (size_t y = 0; y < grid.size.y; y++) {
            for (size_t x = 0; x < grid.size.x; x++) {
                Vec2<size_t> cheat_start_pos{x, y};
                if (distances[cheat_start_pos] == SIZE_MAX)
                    continue;
                for (const auto offset : cheat_end_offsets) {
                    auto cheat_end_pos = cheat_start_pos + offset;
                    if (!grid.pos_is_inside(cheat_end_pos) ||
                        distances[cheat_end_pos] == SIZE_MAX)
                        continue;
                    ptrdiff_t cheat_distance =
                        llabs(offset.x) + llabs(offset.y);
                    ptrdiff_t saving = distances[cheat_end_pos] -
                                       distances[cheat_start_pos] -
                                       cheat_distance;
                    if (saving < (ptrdiff_t)min_saving)
                        continue;
                    count++;
                }
            }
        }
        return count;
    }
};

void part1(FILE* input_) {
    Input input(input_);
    size_t count = input.count_cheats(2, 2, 100);
    printf("%zu\n", count);
}

void part2(FILE* input_) {
    Input input(input_);
    size_t count = input.count_cheats(1, 20, 100);
    printf("%zu\n", count);
}
