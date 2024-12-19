#include "../common.hh"
#include <cstdio>
#include <cstring>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

struct Input {
    std::vector<std::string> patterns, designs;
    std::unordered_map<std::string_view, size_t> cache;

    Input(FILE* input) {
        std::string pattern;
        for (int c; (c = read_char(input)) != EOF && c != '\n';) {
            if (c == ',') {
                if (!pattern.empty())
                    patterns.push_back(pattern);
                pattern.clear();
            } else if (c != ' ') {
                pattern.push_back(c);
            }
        }
        if (!pattern.empty())
            patterns.push_back(pattern);
        read_line(input);
        for (std::string design; !(design = read_line(input)).empty();) {
            if (design.back() == '\n')
                design.pop_back();
            designs.push_back(design);
        }
    }

    size_t count_possible(const std::string_view& design) {
        if (design.size() == 0)
            return 1;
        if (cache.count(design))
            return cache[design];
        size_t count = 0;
        for (const auto& pattern : patterns) {
            if (design.compare(0, pattern.size(), pattern))
                continue;
            count += count_possible(design.substr(pattern.size()));
        }
        cache[design] = count;
        return count;
    }
};

void part1(FILE* input_) {
    Input input(input_);
    size_t possible = 0;
    for (const auto& design : input.designs) {
        possible += input.count_possible(design) > 0;
    }
    printf("%zu\n", possible);
}

void part2(FILE* input_) {
    Input input(input_);
    size_t sum = 0;
    for (const auto& design : input.designs) {
        sum += input.count_possible(design);
    }
    printf("%zu\n", sum);
}
