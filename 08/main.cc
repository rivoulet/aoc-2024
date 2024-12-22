#include "../common.hh"
#include <cassert>
#include <cstddef>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <vector>

std::pair<std::unordered_map<char, std::vector<Vec2<size_t>>>, Vec2<size_t>>
parse_grid(FILE* input) {
    std::unordered_map<char, std::vector<Vec2<size_t>>> antennae;
    Vec2<size_t> size{0, 0};
    size_t cur_width = 0;
    for (int c; (c = read_char(input)) != EOF;) {
        if (c == '\n') {
            if (size.x) {
                assert(cur_width == size.x);
            } else {
                size.x = cur_width;
            }
            cur_width = 0;
            size.y++;
        } else {
            if (c != '.') {
                antennae[c].push_back({cur_width, size.y});
            }
            cur_width++;
        }
    }
    return {antennae, size};
}

void part1(FILE* input) {
    auto [antennae_by_kind, grid_size] = parse_grid(input);
    std::unordered_set<Vec2<size_t>> antinodes;
    for (const auto& [kind, antennae] : antennae_by_kind) {
        for (size_t i = 0; i + 1 < antennae.size(); i++) {
            for (size_t j = i + 1; j < antennae.size(); j++) {
                Vec2<ptrdiff_t> a = antennae[i], b = antennae[j],
                                distance = b - a, antinode_a = a - distance,
                                antinode_b = b + distance;
                if ((size_t)antinode_a.x < grid_size.x &&
                    (size_t)antinode_a.y < grid_size.y) {
                    antinodes.emplace(antinode_a);
                }
                if ((size_t)antinode_b.x < grid_size.x &&
                    (size_t)antinode_b.y < grid_size.y) {
                    antinodes.emplace(antinode_b);
                }
            }
        }
    }
    printf("%zu\n", antinodes.size());
}

void part2(FILE* input) {
    auto [antennae_by_kind, grid_size] = parse_grid(input);
    std::unordered_set<Vec2<size_t>> antinodes;
    for (const auto& [kind, antennae] : antennae_by_kind) {
        for (size_t i = 0; i + 1 < antennae.size(); i++) {
            for (size_t j = i + 1; j < antennae.size(); j++) {
                Vec2<ptrdiff_t> a = antennae[i], b = antennae[j],
                                distance = b - a;
                distance /= std::gcd(distance.x, distance.y);
                Vec2<ptrdiff_t> cur_pos = a;
                do {
                    antinodes.emplace(cur_pos);
                } while ((size_t)(cur_pos -= distance).x < grid_size.x &&
                         (size_t)cur_pos.y < grid_size.y);
                while ((size_t)(cur_pos += distance).x < grid_size.x &&
                       (size_t)cur_pos.y < grid_size.y) {
                    antinodes.emplace(cur_pos);
                }
            }
        }
    }
    printf("%zu\n", antinodes.size());
}
