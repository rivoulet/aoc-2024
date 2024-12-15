#include "../common.hh"
#include <cstddef>
#include <cstdio>
#include <vector>

enum class Cell {
    Empty,
    Wall,
    Box,
    BoxL,
    BoxR,
};

struct Input {
    Grid<Cell> grid;
    Vec2<size_t> robot_pos;
    std::vector<GridDir> dirs;

    Input(FILE* input_)
        : grid(input_, [&](char c, Vec2<size_t> pos) {
              switch (c) {
              case '.':
                  return Cell::Empty;
              case '#':
                  return Cell::Wall;
              case 'O':
                  return Cell::Box;
              default: {
                  robot_pos = pos;
                  return Cell::Empty;
              }
              }
          }) {
        for (int c; (c = read_char(input_)) != EOF;) {
            switch (c) {
            case '^': {
                dirs.push_back(GridDir::Up);
                break;
            }
            case '>': {
                dirs.push_back(GridDir::Right);
                break;
            }
            case 'v': {
                dirs.push_back(GridDir::Down);
                break;
            }
            case '<': {
                dirs.push_back(GridDir::Left);
                break;
            }
            }
        }
    }

    void widen() {
        Grid<Cell> new_grid({grid.size.x * 2, grid.size.y});
        for (size_t y = 0; y < grid.size.y; y++) {
            for (size_t x = 0; x < grid.size.x; x++) {
                Vec2<size_t> l = {x * 2, y}, r = {l.x + 1, l.y};
                switch (grid[{x, y}]) {
                case Cell::Empty: {
                    new_grid[l] = new_grid[r] = Cell::Empty;
                    break;
                }
                case Cell::Wall: {
                    new_grid[l] = new_grid[r] = Cell::Wall;
                    break;
                }
                default: {
                    new_grid[l] = Cell::BoxL;
                    new_grid[r] = Cell::BoxR;
                    break;
                }
                }
            }
        }
        grid = new_grid;
        robot_pos.x *= 2;
    }

    bool can_move(Vec2<size_t> pos, GridDir dir) {
        auto new_pos = pos + grid_dir_offset(dir);
        switch (grid[new_pos]) {
        case Cell::Empty: {
            return true;
        }
        case Cell::Wall: {
            return false;
        }
        case Cell::Box: {
            return can_move(new_pos, dir);
        }
        case Cell::BoxL: {
            Vec2<size_t> r = {new_pos.x + 1, new_pos.y};
            return can_move(r, dir) &&
                   (grid_dir_is_horizontal(dir) || can_move(new_pos, dir));
        }
        case Cell::BoxR: {
            Vec2<size_t> l = {new_pos.x - 1, new_pos.y};
            return can_move(l, dir) &&
                   (grid_dir_is_horizontal(dir) || can_move(new_pos, dir));
        }
        }
    }

    void do_move(Vec2<size_t> pos, GridDir dir) {
        auto new_pos = pos + grid_dir_offset(dir);
        switch (grid[new_pos]) {
        case Cell::Box: {
            do_move(new_pos, dir);
            break;
        }
        case Cell::BoxL: {
            do_move(new_pos, dir);
            if (!grid_dir_is_horizontal(dir)) {
                Vec2<size_t> r = {new_pos.x + 1, new_pos.y};
                do_move(r, dir);
                grid[r] = Cell::Empty;
            }
            break;
        }
        case Cell::BoxR: {
            do_move(new_pos, dir);
            if (!grid_dir_is_horizontal(dir)) {
                Vec2<size_t> l = {new_pos.x - 1, new_pos.y};
                do_move(l, dir);
                grid[l] = Cell::Empty;
            }
            break;
        }
        default:;
        }
        grid[new_pos] = grid[pos];
    }

    void step(GridDir dir) {
        if (!can_move(robot_pos, dir))
            return;
        do_move(robot_pos, dir);
        robot_pos += grid_dir_offset(dir);
    }

    void print() {
        for (size_t y = 0; y < grid.size.y; y++) {
            for (size_t x = 0; x < grid.size.x; x++) {
                char c;
                switch (grid[{x, y}]) {
                case Cell::Empty: {
                    c = robot_pos == Vec2<size_t>{x, y} ? '@' : '.';
                    break;
                }
                case Cell::Wall: {
                    c = '#';
                    break;
                }
                case Cell::Box: {
                    c = 'O';
                    break;
                }
                case Cell::BoxL: {
                    c = '[';
                    break;
                }
                case Cell::BoxR: {
                    c = ']';
                    break;
                }
                }
                printf("%c", c);
            }
            printf("\n");
        }
    }
};

void part1(FILE* input_) {
    Input input(input_);
    for (const auto dir : input.dirs) {
        input.step(dir);
    }

    size_t sum = 0;
    for (size_t y = 0; y < input.grid.size.y; y++) {
        for (size_t x = 0; x < input.grid.size.x; x++) {
            if (input.grid[{x, y}] == Cell::Box) {
                sum += 100 * y + x;
            }
        }
    }
    printf("%zu\n", sum);
}

void part2(FILE* input_) {
    Input input(input_);
    input.widen();
    for (const auto dir : input.dirs) {
        input.step(dir);
    }

    size_t sum = 0;
    for (size_t y = 0; y < input.grid.size.y; y++) {
        for (size_t x = 0; x < input.grid.size.x; x++) {
            if (input.grid[{x, y}] == Cell::BoxL) {
                sum += 100 * y + x;
            }
        }
    }
    printf("%zu\n", sum);
}
