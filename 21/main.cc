#include "../common.hh"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <optional>
#include <queue>
#include <unordered_map>
#include <vector>

template <typename T>
void fill_distances_from(const Grid<std::optional<T>>& grid,
                         Vec2<size_t> start_pos, Grid<size_t>& distances,
                         std::queue<Vec2<size_t>>& to_visit) {
    auto try_visit = [&](auto pos, auto distance) {
        if (distances[pos] != SIZE_MAX)
            return;
        distances[pos] = distance;
        to_visit.push(pos);
    };
    distances.fill(SIZE_MAX);
    try_visit(start_pos, 0);
    while (!to_visit.empty()) {
        auto pos = to_visit.front();
        to_visit.pop();
        for (const auto dir : grid_dirs) {
            auto new_pos = pos + grid_dir_offset(dir);
            if (grid.pos_is_inside(new_pos) && grid[new_pos].has_value())
                try_visit(new_pos, distances[pos] + 1);
        }
    }
}

void get_shortest_path_to(const Grid<size_t>& distances, Vec2<size_t> end_pos,
                          std::vector<GridDir>& cur_suffix,
                          std::vector<std::vector<GridDir>>& out) {
    auto distance = distances[end_pos];
    if (distance == 0) {
        out.push_back(cur_suffix);
        return;
    }
    for (const auto dir : grid_dirs) {
        auto prev_pos = end_pos - grid_dir_offset(dir);
        if (!distances.pos_is_inside(prev_pos) ||
            distances[prev_pos] + 1 != distance)
            continue;
        cur_suffix.insert(cur_suffix.begin(), dir);
        get_shortest_path_to(distances, prev_pos, cur_suffix, out);
        cur_suffix.erase(cur_suffix.begin());
    }
}

template <typename T>
Grid<Grid<std::vector<std::vector<GridDir>>>>
get_all_shortest_paths(const Grid<std::optional<T>>& grid) {
    Grid<Grid<std::vector<std::vector<GridDir>>>> result(grid.size, grid.size);
    Grid<size_t> distances(grid.size);
    std::queue<Vec2<size_t>> to_visit;
    for (size_t start_y = 0; start_y < grid.size.y; start_y++) {
        for (size_t start_x = 0; start_x < grid.size.x; start_x++) {
            Vec2<size_t> start_pos{start_x, start_y};
            if (!grid[start_pos].has_value())
                continue;
            fill_distances_from(grid, start_pos, distances, to_visit);
            for (size_t end_y = 0; end_y < grid.size.y; end_y++) {
                for (size_t end_x = 0; end_x < grid.size.x; end_x++) {
                    Vec2<size_t> end_pos{end_x, end_y};
                    if (end_pos == start_pos || !grid[end_pos].has_value())
                        continue;
                    auto& paths = result[start_pos][end_pos];
                    {
                        std::vector<GridDir> cur_suffix;
                        get_shortest_path_to(distances, end_pos, cur_suffix,
                                             paths);
                    }
                    paths.erase(
                        std::remove_if(
                            paths.begin(), paths.end(),
                            [](const auto& path) {
                                bool finished_dirs[4] = {false, false, false,
                                                         false};
                                GridDir last_dir;
                                for (size_t i = 0; i < path.size(); i++) {
                                    auto dir = path[i];
                                    if (finished_dirs[(size_t)dir])
                                        return true;
                                    if (i && dir != last_dir) {
                                        finished_dirs[(size_t)last_dir] = true;
                                    }
                                    last_dir = dir;
                                }
                                return false;
                            }),
                        paths.end());
                }
            }
        }
    }
    return result;
}

enum class DirKey {
    Right,
    Left,
    Down,
    Up,
    Activate,
};

constexpr Vec2<size_t> num_keypad_start_pos{2, 3};
constexpr Vec2<size_t> dir_keypad_start_pos{2, 0};

template <typename I>
void get_shortest_paths_to_type_sequence(
    const Grid<std::optional<DirKey>>& dir_keypad,
    const Grid<Grid<std::vector<std::vector<GridDir>>>>& dir_keypad_all_paths,
    const I& sequence, std::vector<std::vector<DirKey>>& out) {
    auto out_start_i = out.size();
    auto pos = dir_keypad_start_pos;
    out.push_back({});
    auto append = [&](const auto element) {
        auto new_pos = dir_keypad.find((DirKey)element);
        assert(new_pos != dir_keypad.size);
        const auto& new_paths = dir_keypad_all_paths[pos][new_pos];
        size_t out_end_i = out.size();
        for (size_t i = new_paths.size(); i--;) {
            for (size_t j = out_start_i; j < out_end_i; j++) {
                std::vector<DirKey>* result;
                if (i) {
                    out.push_back(out[j]);
                    result = &out.back();
                } else {
                    result = &out[j];
                }
                for (auto dir : new_paths[i]) {
                    result->push_back((DirKey)dir);
                }
                result->push_back(DirKey::Activate);
            }
        }
        pos = new_pos;
    };
    for (const auto& element : sequence) {
        append(element);
    }
    append(DirKey::Activate);
}

template <typename I>
void get_shortest_paths_to_type_sequences(
    const Grid<std::optional<DirKey>>& dir_keypad,
    const Grid<Grid<std::vector<std::vector<GridDir>>>>& dir_keypad_all_paths,
    const I& sequences, std::vector<std::vector<DirKey>>& out) {
    out.clear();
    size_t min_size = SIZE_MAX;
    for (const auto& sequence : sequences) {
        get_shortest_paths_to_type_sequence(dir_keypad, dir_keypad_all_paths,
                                            sequence, out);
        if (!out.empty())
            min_size = std::min(min_size, out.back().size());
    }
    out.erase(std::remove_if(
                  out.begin(), out.end(),
                  [&](const auto& path) { return path.size() != min_size; }),
              out.end());
}

template <typename A, typename B>
bool compare_recursive_path_sizes_to_type_sequences(
    const Grid<std::optional<DirKey>>& dir_keypad,
    const Grid<Grid<std::vector<std::vector<GridDir>>>>& dir_keypad_all_paths,
    const A& a, const B& b) {
    std::array<std::vector<std::vector<DirKey>>, 2> a_paths, b_paths;
    get_shortest_paths_to_type_sequence(dir_keypad, dir_keypad_all_paths, a,
                                        a_paths[0]);
    get_shortest_paths_to_type_sequence(dir_keypad, dir_keypad_all_paths, b,
                                        b_paths[0]);
    for (size_t i = 0;; i ^= 1) {
        const auto& a_paths_ = a_paths[i];
        const auto& b_paths_ = b_paths[i];
        auto& new_a_paths = a_paths[i ^ 1];
        auto& new_b_paths = b_paths[i ^ 1];
        if (a_paths_[0].size() < b_paths_[0].size())
            return true;
        if (a_paths_[0].size() > b_paths_[0].size())
            return false;
        get_shortest_paths_to_type_sequences(dir_keypad, dir_keypad_all_paths,
                                             a_paths_, new_a_paths);
        get_shortest_paths_to_type_sequences(dir_keypad, dir_keypad_all_paths,
                                             b_paths_, new_b_paths);
    }
    return true;
}

void get_shortest_paths_to_type(
    const Grid<Grid<std::vector<std::vector<GridDir>>>>& paths,
    const Grid<std::optional<DirKey>>& dir_keypad,
    const Grid<Grid<std::vector<std::vector<GridDir>>>>& dir_keypad_all_paths,
    Grid<Grid<std::optional<std::vector<GridDir>>>>& out) {
    for (size_t start_y = 0; start_y < paths.size.y; start_y++) {
        for (size_t start_x = 0; start_x < paths.size.x; start_x++) {
            Vec2<size_t> start_pos{start_x, start_y};
            for (size_t end_y = 0; end_y < paths.size.y; end_y++) {
                for (size_t end_x = 0; end_x < paths.size.x; end_x++) {
                    Vec2<size_t> end_pos{end_x, end_y};
                    if (end_pos == start_pos)
                        continue;
                    const auto& paths_ = paths[start_pos][end_pos];
                    auto& out_ = out[start_pos][end_pos];
                    if (paths_.empty()) {
                        out_ = {};
                        continue;
                    }
                    out_ = *std::min_element(
                        paths_.begin(), paths_.end(),
                        [&](const auto& a, const auto& b) {
                            return compare_recursive_path_sizes_to_type_sequences(
                                dir_keypad, dir_keypad_all_paths, a, b);
                        });
                }
            }
        }
    }
}

struct Code {
    size_t num;
    std::string chars;
};

struct Input {
    Grid<std::optional<char>> num_keypad;
    Grid<std::optional<DirKey>> dir_keypad;
    Grid<Grid<std::optional<std::vector<GridDir>>>> num_keypad_paths,
        dir_keypad_paths;
    std::vector<Code> codes;

    Input(FILE* input)
        : num_keypad({3, 4}), dir_keypad({3, 2}),
          num_keypad_paths(num_keypad.size, num_keypad.size),
          dir_keypad_paths(dir_keypad.size, dir_keypad.size) {
        for (size_t y = 0; y < 3; y++) {
            for (size_t x = 0; x < 3; x++) {
                num_keypad[{x, y}] = '1' + (2 - y) * 3 + x;
            }
        }
        num_keypad[{1, 3}] = '0';
        num_keypad[{2, 3}] = 'A';

        dir_keypad[{1, 0}] = DirKey::Up;
        dir_keypad[{2, 0}] = DirKey::Activate;
        dir_keypad[{0, 1}] = DirKey::Left;
        dir_keypad[{1, 1}] = DirKey::Down;
        dir_keypad[{2, 1}] = DirKey::Right;

        for (auto& line : read_all_lines(input)) {
            if (line.empty())
                continue;
            if (line.back() == '\n')
                line.pop_back();
            codes.push_back(Code{(size_t)atoll(line.c_str()), line});
        }

        Grid<Grid<std::vector<std::vector<GridDir>>>> num_keypad_all_paths =
            get_all_shortest_paths(num_keypad);
        Grid<Grid<std::vector<std::vector<GridDir>>>> dir_keypad_all_paths =
            get_all_shortest_paths(dir_keypad);
        get_shortest_paths_to_type(num_keypad_all_paths, dir_keypad,
                                   dir_keypad_all_paths, num_keypad_paths);
        get_shortest_paths_to_type(dir_keypad_all_paths, dir_keypad,
                                   dir_keypad_all_paths, dir_keypad_paths);
    }

    template <typename T, typename I>
    size_t get_shortest_recursive_path_size_to_type_sequence(
        const Grid<std::optional<T>>& grid,
        const Grid<Grid<std::optional<std::vector<GridDir>>>>& paths,
        Vec2<size_t> start_pos, const I& sequence, size_t remaining,
        std::unordered_map<std::pair<const void*, size_t>, size_t>& cache,
        bool append_activate = false) {
        if (remaining == 0) {
            return sequence.size() + append_activate;
        }
        if (cache.count({&sequence, remaining}))
            return cache[{&sequence, remaining}];
        size_t result = 0;
        auto pos = start_pos;
        auto append = [&](const auto& element) {
            auto new_pos = grid.find((T)element);
            assert(new_pos != grid.size);
            const auto& new_path = paths[pos][new_pos];
            if (new_path.has_value()) {
                auto size = get_shortest_recursive_path_size_to_type_sequence(
                    dir_keypad, dir_keypad_paths, dir_keypad_start_pos,
                    new_path.value(), remaining - 1, cache, true);
                result += size;
            } else {
                result++;
            }
            pos = new_pos;
        };
        for (const auto& element : sequence) {
            append(element);
        }
        if (append_activate)
            append(DirKey::Activate);
        return cache[{&sequence, remaining}] = result;
    }
};

void part1(FILE* input_) {
    Input input(input_);
    size_t sum = 0;
    for (const auto& code : input.codes) {
        std::unordered_map<std::pair<const void*, size_t>, size_t> cache;
        size_t size = input.get_shortest_recursive_path_size_to_type_sequence(
            input.num_keypad, input.num_keypad_paths, num_keypad_start_pos,
            code.chars, 3, cache);
        sum += code.num * size;
    }
    printf("%zu\n", sum);
}

void part2(FILE* input_) {
    Input input(input_);
    size_t sum = 0;
    for (const auto& code : input.codes) {
        std::unordered_map<std::pair<const void*, size_t>, size_t> cache;
        size_t size = input.get_shortest_recursive_path_size_to_type_sequence(
            input.num_keypad, input.num_keypad_paths, num_keypad_start_pos,
            code.chars, 26, cache);
        sum += code.num * size;
    }
    printf("%zu\n", sum);
}
