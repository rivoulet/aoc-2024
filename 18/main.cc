#include "../common.hh"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <queue>
#include <utility>
#include <vector>

enum class Cell {
    Empty,
    Obstacle,
};

struct Input {
    Grid<Cell> grid;
    std::vector<Vec2<size_t>> bytes;

    static constexpr Vec2<size_t> start_pos{0, 0};
    static constexpr Vec2<size_t> size{71, 71};
    static constexpr Vec2<size_t> end_pos = size - 1;

    Input(FILE* input) : grid(size, Cell::Empty) {
        for (Vec2<size_t> pos; fscanf(input, "%zu,%zu", &pos.x, &pos.y) == 2;) {
            assert(pos.x < size.x && pos.y < size.y);
            bytes.push_back(pos);
        }
    }

    std::optional<size_t> find_min_distance() {
        Grid<bool> visited(size, false);

        using VisitNode = std::pair<Vec2<size_t>, size_t>;
        auto compare = [&](const VisitNode& a, const VisitNode& b) {
            return std::get<1>(a) > std::get<1>(b);
        };
        std::priority_queue<VisitNode, std::vector<VisitNode>,
                            decltype(compare)>
            to_visit(compare);

        auto try_visit = [&](auto pos, auto distance) {
            if (!visited[pos])
                to_visit.push({pos, distance});
        };

        try_visit(start_pos, 0);
        while (!to_visit.empty()) {
            auto [pos, distance] = to_visit.top();
            to_visit.pop();

            if (pos == end_pos) {
                return distance;
            }

            if (visited[pos])
                continue;
            visited[pos] = true;

            for (auto dir : grid_dirs) {
                auto new_pos = pos + grid_dir_offset(dir);
                if (grid.pos_is_inside(new_pos) && grid[new_pos] == Cell::Empty)
                    try_visit(new_pos, distance + 1);
            }
        }

        return {};
    }
};

void part1(FILE* input_) {
    Input input(input_);
    for (size_t i = 0; i < std::min((size_t)1024, input.bytes.size()); i++) {
        input.grid[input.bytes[i]] = Cell::Obstacle;
    }
    auto distance = input.find_min_distance();
    assert(distance.has_value());
    printf("%zu\n", distance.value());
}

void part2(FILE* input_) {
    Input input(input_);
    size_t i = 0;
    for (; i < input.bytes.size(); i++) {
        input.grid[input.bytes[i]] = Cell::Obstacle;
        if (!input.find_min_distance().has_value())
            break;
    }
    assert(i < input.bytes.size());
    auto pos = input.bytes[i];
    printf("%zu,%zu\n", pos.x, pos.y);
}
