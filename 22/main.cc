#include "../common.hh"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <unordered_map>
#include <vector>

size_t step_number(size_t n) {
    n ^= (n << 6) & 0xFF'FFFF;
    n ^= n >> 5;
    n ^= (n << 11) & 0xFF'FFFF;
    return n;
}

struct Input {
    std::vector<size_t> numbers;

    Input(FILE* input) {
        for (std::string line; !(line = read_line(input)).empty();) {
            numbers.push_back(atoll(line.c_str()));
        }
    }
};

constexpr size_t steps = 2000;

void part1(FILE* input_) {
    Input input(input_);
    size_t sum = 0;
    for (auto number : input.numbers) {
        for (size_t i = 0; i < steps; i++) {
            number = step_number(number);
        }
        sum += number;
    }
    printf("%zu\n", sum);
}

constexpr size_t win_size = 4;
constexpr size_t max_abs_diff = 9999;
using AbsDiff = uint16_t;

void part2(FILE* input_) {
    Input input(input_);

    std::vector<std::vector<std::pair<uint8_t, int8_t>>> history;
    for (auto number : input.numbers) {
        history.emplace_back();
        auto& history_ = history.back();
        for (size_t i = 0; i < steps; i++) {
            auto new_number = step_number(number);
            history_.emplace_back(new_number % 10,
                                  new_number % 10 - number % 10);
            number = new_number;
        }
    }

    std::vector<std::unordered_map<std::pair<AbsDiff, uint8_t>, uint8_t>>
        prices_for_diffs;
    for (const auto& history_ : history) {
        prices_for_diffs.emplace_back();
        auto& prices_for_diffs_ = prices_for_diffs.back();
        for (size_t win_start = 0; win_start < steps - (win_size - 1);
             win_start++) {
            AbsDiff abs_diff = 0;
            uint8_t signs = 0;
            for (size_t i = 0; i < win_size; i++) {
                auto diff = history_[win_start + i].second;
                abs_diff = abs_diff * 10 + abs(diff);
                signs = (signs << 1) | (diff < 0);
            }
            prices_for_diffs_.emplace(std::pair{abs_diff, signs},
                                      history_[win_start + win_size - 1].first);
        }
    }

    size_t max_sum = 0;
    for (AbsDiff abs_diff = 0; abs_diff <= max_abs_diff; abs_diff++) {
        for (uint8_t signs = 0; signs <= (1 << win_size) - 1; signs++) {
            auto key = std::pair{abs_diff, signs};
            size_t sum = 0;
            for (const auto& prices_for_diffs_ : prices_for_diffs) {
                sum += prices_for_diffs_.count(key)
                           ? prices_for_diffs_.find(key)->second
                           : 0;
            }
            max_sum = std::max(sum, max_sum);
        }
    }
    printf("%zu\n", max_sum);
}
