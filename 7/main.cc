#include "../common.hh"
#include <cassert>
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <cstdio>

int64_t parse_line(const std::string& line, std::vector<int64_t>& ns) {
    ns.clear();

    unsigned read_chars;
    size_t i = 0;

    int64_t result;
    if (sscanf(line.c_str(), "%" SCNi64 ": %n", &result, &read_chars) < 1)
        return -1;
    i += read_chars;

    for (int64_t n;
         sscanf(line.c_str() + i, "%" SCNi64 "%n", &n, &read_chars) >= 1;
         i += read_chars) {
        ns.push_back(n);
    }
    assert(ns.size() >= 1);

    return result;
}

template <typename T, typename U> T powi(T base, U exp) {
    T result = base;
    while (--exp)
        result *= base;
    return result;
}

void part1(FILE* input) {
    auto lines = read_all_lines(input);

    std::vector<int64_t> ns;

    int64_t sum = 0;

    for (const auto& line : lines) {
        int64_t result = parse_line(line, ns);
        if (result < 0)
            break;

        for (size_t comb = 0; comb < 1 << (ns.size() - 1); comb++) {
            int64_t comb_result = ns[0];
            size_t comb_ = comb;
            for (size_t i = 1; i < ns.size(); i++) {
                if (comb_ & 1) {
                    comb_result *= ns[i];
                } else {
                    comb_result += ns[i];
                }
                comb_ >>= 1;
            }
            if (comb_result == result) {
                sum += result;
                goto next_line;
            }
        }

    next_line:;
    }

    printf("%" PRIi64 "\n", sum);
}

void part2(FILE* input) {
    auto lines = read_all_lines(input);

    std::vector<int64_t> ns;

    int64_t sum = 0;

    for (const auto& line : lines) {
        int64_t result = parse_line(line, ns);
        if (result < 0)
            break;

        size_t combs = powi(3, ns.size() - 1);
        for (size_t comb = 0; comb < combs; comb++) {
            int64_t comb_result = ns[0];
            size_t comb_ = comb;
            for (size_t i = 1; i < ns.size(); i++) {
                switch (comb_ % 3) {
                case 0: {
                    comb_result += ns[i];
                    break;
                }
                case 1: {
                    comb_result *= ns[i];
                    break;
                }
                case 2: {
                    int64_t factor = 10;
                    while (factor <= ns[i]) {
                        factor *= 10;
                    }
                    comb_result = comb_result * factor + ns[i];
                    break;
                }
                }
                comb_ /= 3;
            }
            if (comb_result == result) {
                sum += result;
                goto next_line;
            }
        }

    next_line:;
    }

    printf("%" PRIi64 "\n", sum);
}
