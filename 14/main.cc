#include "../common.hh"
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <vector>

struct Robot {
    Vec2<ptrdiff_t> pos;
    Vec2<ptrdiff_t> velocity;
};

struct RobotGrid {
    Vec2<ptrdiff_t> size;
    std::vector<Robot> robots;

    void step(size_t count) {
        for (auto& robot : robots) {
            robot.pos =
                (size +
                 (robot.pos + robot.velocity * (ptrdiff_t)count) % size) %
                size;
        }
    }

    size_t count_quadrants() {
        size_t counts[4] = {0, 0, 0, 0};
        Vec2<size_t> center = size / 2;
        for (const auto& robot : robots) {
            if (robot.pos.x == center.x || robot.pos.y == center.y)
                continue;
            counts[(robot.pos.y > center.y) * 2 + (robot.pos.x > center.x)]++;
        }
        return counts[0] * counts[1] * counts[2] * counts[3];
    }

    void write_grid(Grid<bool>& grid) {
        assert(grid.size == size);
        grid.fill(false);
        for (const auto& robot : robots) {
            grid[robot.pos] = true;
        }
    }
};

RobotGrid parse_input(FILE* input, Vec2<size_t> size) {
    auto lines = read_all_lines(input);
    RobotGrid result{size, {}};
    for (const auto& line : lines) {
        Robot robot;
        if (sscanf(line.c_str(), "p=%zu,%zu v=%zd,%zd", &robot.pos.x,
                   &robot.pos.y, &robot.velocity.x, &robot.velocity.y) < 4)
            break;
        result.robots.push_back(robot);
    }
    return result;
}

void part1(FILE* input_) {
    auto input = parse_input(input_, {101, 103});
    input.step(100);
    size_t result = input.count_quadrants();
    printf("%zu\n", result);
}

void part2(FILE* input_) {
    auto input = parse_input(input_, {101, 103});
    Grid<bool> grid(input.size);
    for (size_t step = 1;; step++) {
        input.step(1);
        input.write_grid(grid);
        for (size_t y_start = 0; y_start + 2 < input.size.y; y_start++) {
            for (size_t x_start = 0; x_start + 2 < input.size.x; x_start++) {
                if (!grid[{x_start, y_start}] ||
                    !grid[{x_start, y_start + 1}] ||
                    !grid[{x_start + 1, y_start}])
                    continue;
                for (size_t y_end = y_start + 2; y_end < input.size.y;
                     y_end++) {
                    if (!grid[{x_start, y_end}])
                        break;
                    for (size_t x_end = x_start + 2; x_end < input.size.x;
                         x_end++) {
                        if (!grid[{x_end, y_start}])
                            break;
                        for (size_t x = x_start; x <= x_end; x++) {
                            if (!grid[{x, y_end}])
                                goto failed;
                        }
                        for (size_t y = y_start; y <= y_end; y++) {
                            if (!grid[{x_end, y}])
                                goto failed;
                        }
                        goto found_square;
                    failed:;
                    }
                }
            }
        }
        continue;
    found_square:;
        for (size_t y = 0; y < input.size.y; y++) {
            for (size_t x = 0; x < input.size.x; x++) {
                printf("%c", grid[{x, y}] ? 'x' : '.');
            }
            printf("\n");
        }
        printf("Step %zu\n", step);
        getchar();
    }
}
