#include "../common.hh"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <queue>
#include <tuple>
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

    size_t find_best_path_score() {
        Grid<uint8_t> visit_masks(grid.size, 0);

        using VisitNode = std::tuple<Vec2<size_t>, GridDir, size_t>;
        auto compare = [&](const VisitNode& a, const VisitNode& b) {
            return std::get<2>(a) > std::get<2>(b);
        };
        std::priority_queue<VisitNode, std::vector<VisitNode>,
                            decltype(compare)>
            to_visit(compare);

        auto try_visit = [&](auto pos, auto dir, auto distance) {
            if (!(visit_masks[pos] & grid_dir_mask(dir))) {
                to_visit.push({pos, dir, distance});
            }
        };

        try_visit(start_pos, GridDir::Right, 0);
        while (!to_visit.empty()) {
            auto [pos, dir, distance] = to_visit.top();
            to_visit.pop();
            auto dir_mask = grid_dir_mask(dir);

            if (pos == end_pos) {
                return distance;
            }

            if (visit_masks[pos] & dir_mask)
                continue;
            visit_masks[pos] |= dir_mask;

            auto new_pos = pos + grid_dir_offset(dir);
            if (grid.pos_is_inside(new_pos) && grid[new_pos] == Cell::Empty)
                try_visit(new_pos, dir, distance + 1);
            try_visit(pos, rotate_grid_dir_clockwise(dir), distance + 1000);
            try_visit(pos, rotate_grid_dir_counterclockwise(dir),
                      distance + 1000);
        }

        return SIZE_MAX;
    }

    size_t count_sitting_spots() {
        Grid<uint8_t> visit_masks(grid.size, 0);
        Grid<std::array<size_t, 4>> distances(
            grid.size, std::array{SIZE_MAX, SIZE_MAX, SIZE_MAX, SIZE_MAX});

        {
            using VisitNode = std::tuple<Vec2<size_t>, GridDir, size_t>;
            auto compare = [&](const VisitNode& a, const VisitNode& b) {
                return std::get<2>(a) > std::get<2>(b);
            };
            std::priority_queue<VisitNode, std::vector<VisitNode>,
                                decltype(compare)>
                to_visit(compare);

            auto try_visit = [&](auto pos, auto dir, auto distance) {
                if (!(visit_masks[pos] & grid_dir_mask(dir))) {
                    to_visit.push({pos, dir, distance});
                }
            };

            try_visit(start_pos, GridDir::Right, 0);
            while (!to_visit.empty()) {
                auto [pos, dir, distance] = to_visit.top();
                to_visit.pop();
                auto dir_mask = grid_dir_mask(dir);

                if (distance < distances[pos][(size_t)dir]) {
                    distances[pos][(size_t)dir] = distance;
                }

                if (visit_masks[pos] & dir_mask)
                    continue;
                visit_masks[pos] |= dir_mask;

                auto new_pos = pos + grid_dir_offset(dir);
                if (grid.pos_is_inside(new_pos) && grid[new_pos] == Cell::Empty)
                    try_visit(new_pos, dir, distance + 1);
                try_visit(pos, rotate_grid_dir_clockwise(dir), distance + 1000);
                try_visit(pos, rotate_grid_dir_counterclockwise(dir),
                          distance + 1000);
            }
        }
        visit_masks.fill(0);
        {
            std::vector<std::pair<Vec2<size_t>, GridDir>> to_visit;

            auto try_visit = [&](auto pos, auto dir) {
                auto dir_mask = grid_dir_mask(dir);
                if (!(visit_masks[pos] & dir_mask)) {
                    visit_masks[pos] |= dir_mask;
                    to_visit.push_back({pos, dir});
                }
            };

            const auto& end_distances = distances[end_pos];
            size_t min_end_distance =
                *std::min_element(end_distances.begin(), end_distances.end());
            for (auto dir : grid_dirs) {
                if (end_distances[(size_t)dir] == min_end_distance)
                    try_visit(end_pos, dir);
            }

            while (!to_visit.empty()) {
                auto [pos, dir] = to_visit.back();
                to_visit.pop_back();

                auto distance = distances[pos][(size_t)dir];
                if (!distance)
                    continue;

                auto prev_pos = pos - grid_dir_offset(dir);
                if (grid.pos_is_inside(prev_pos) &&
                    distances[prev_pos][(size_t)dir] + 1 == distance)
                    try_visit(prev_pos, dir);

                auto cw = rotate_grid_dir_clockwise(dir);
                if (distances[pos][(size_t)cw] + 1000 == distance)
                    try_visit(pos, cw);

                auto ccw = rotate_grid_dir_counterclockwise(dir);
                if (distances[pos][(size_t)ccw] + 1000 == distance)
                    try_visit(pos, ccw);
            }
        }
        return std::count_if(visit_masks.contents.begin(),
                             visit_masks.contents.end(),
                             [](auto mask) { return mask; });
    }
};

void part1(FILE* input_) {
    Input input(input_);
    size_t score = input.find_best_path_score();
    printf("%zu\n", score);
}

void part2(FILE* input_) {
    Input input(input_);
    size_t spots = input.count_sitting_spots();
    printf("%zu\n", spots);
}
