#include "../common.hh"
#include <cstdint>
#include <cstdlib>
#include <unordered_map>
#include <vector>

std::vector<uint64_t> parse_input(FILE* input_) {
    std::string input = read_all(input_);
    std::vector<uint64_t> result;
    for (size_t i = 0;;) {
        result.push_back(strtoull(&input.c_str()[i], NULL, 10));
        i = input.find(' ', i + 1);
        if (i == std::string::npos)
            break;
    }
    return result;
}

uint8_t digits(uint64_t value) {
    uint8_t digits = 0;
    do {
        digits += 1;
        value /= 10;
    } while (value > 0);
    return digits;
}

uint64_t exp10(uint8_t exp) {
    uint64_t v = 1;
    while (exp--)
        v *= 10;
    return v;
}

size_t pass(uint64_t value, size_t remaining_passes) {
    if (remaining_passes == 0) {
        return 1;
    }
    if (value == 0) {
        return pass(1, remaining_passes - 1);
    }
    uint8_t digits_ = digits(value);
    if ((digits_ & 1) == 0) {
        uint64_t factor = exp10(digits_ / 2);
        return pass(value % factor, remaining_passes - 1) +
               pass(value / factor, remaining_passes - 1);
    }
    return pass(value * 2024, remaining_passes - 1);
}

using cache = std::unordered_map<std::pair<uint64_t, size_t>, size_t>;

size_t pass_cached(uint64_t value, size_t remaining_passes, cache& cache);

size_t pass_cached_inner(uint64_t value, size_t remaining_passes,
                         cache& cache) {
    if (remaining_passes == 0) {
        return 1;
    }
    if (value == 0) {
        return pass_cached(1, remaining_passes - 1, cache);
    }
    uint8_t digits_ = digits(value);
    if ((digits_ & 1) == 0) {
        uint64_t factor = exp10(digits_ / 2);
        return pass_cached(value % factor, remaining_passes - 1, cache) +
               pass_cached(value / factor, remaining_passes - 1, cache);
    }
    return pass_cached(value * 2024, remaining_passes - 1, cache);
}

size_t pass_cached(uint64_t value, size_t remaining_passes, cache& cache) {
    std::pair<uint64_t, size_t> cache_key = {value, remaining_passes};
    if (cache.count(cache_key)) {
        return cache[cache_key];
    }
    auto result = pass_cached_inner(value, remaining_passes, cache);
    cache[cache_key] = result;
    return result;
}

void part1(FILE* input_) {
    auto input = parse_input(input_);
    size_t count = 0;
    for (const auto value : input) {
        count += pass(value, 25);
    }
    printf("%zu\n", count);
}

void part2(FILE* input_) {
    auto input = parse_input(input_);
    size_t count = 0;
    cache cache;
    for (const auto value : input) {
        count += pass_cached(value, 75, cache);
    }
    printf("%zu\n", count);
}
