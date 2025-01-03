#include "../common.hh"
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <sys/types.h>
#include <utility>
#include <vector>

struct Guard {
    GridDir dir;
    Vec2<size_t> pos;
};

struct Cell {
    bool is_obstacle;
    uint8_t visit_dir_mask;
};

std::pair<Grid<Cell>, Guard> parse_map(FILE* input_) {
    std::string input = read_all(input_);
    Grid<Cell> grid = {{}, {0, 0}};
    Guard guard;
    size_t x = 0;
    for (char c : input) {
        switch (c) {
        case '.': {
            grid.contents.push_back({false, 0});
            x++;
            continue;
        }
        case '#': {
            grid.contents.push_back({true, 0});
            x++;
            continue;
        }
        case '\n': {
            if (grid.size.x) {
                assert(x == grid.size.x);
            } else {
                grid.size.x = x;
            }
            x = 0;
            grid.size.y++;
            continue;
        }
        case '^': {
            guard.dir = GridDir::Up;
            break;
        }
        case '>': {
            guard.dir = GridDir::Right;
            break;
        }
        case 'v': {
            guard.dir = GridDir::Down;
            break;
        }
        case '<': {
            guard.dir = GridDir::Left;
            break;
        }
        }
        guard.pos = {x, grid.size.y};
        grid.contents.push_back({false, (uint8_t)(1 << (int)guard.dir)});
        x++;
    }
    return {grid, guard};
}

bool simulate(Grid<Cell>& grid, Guard guard) {
    for (;;) {
        auto new_pos = guard.pos + grid_dir_offset(guard.dir);
        if (new_pos.x >= grid.size.x || new_pos.y >= grid.size.y)
            return true;
        if (grid[new_pos].is_obstacle) {
            guard.dir = rotate_grid_dir_clockwise(guard.dir);
        } else {
            guard.pos = new_pos;
        }
        Cell& cell = grid[guard.pos];
        uint8_t visit_dir_mask = 1 << (uint8_t)guard.dir;
        if ((cell.visit_dir_mask & visit_dir_mask) == 0) {
            cell.visit_dir_mask |= visit_dir_mask;
        } else {
            return false;
        }
    }
}

void part1(FILE* input) {
    auto [grid, guard] = parse_map(input);
    assert(simulate(grid, guard));
    size_t visited = 0;
    for (size_t i = 0; i < grid.contents.size(); i++) {
        visited += grid.contents[i].visit_dir_mask != 0;
    }
    printf("%zu\n", visited);
}

void part2(FILE* input) {
    auto [grid, guard] = parse_map(input);
    size_t loops = 0;
    for (size_t y = 0; y < grid.size.y; y++) {
        for (size_t x = 0; x < grid.size.x; x++) {
            if (grid[{x, y}].is_obstacle)
                continue;
            auto grid_with_obstacle = grid;
            grid_with_obstacle[{x, y}].is_obstacle = true;
            loops += !simulate(grid_with_obstacle, guard);
        }
    }
    printf("%zu\n", loops);
}
