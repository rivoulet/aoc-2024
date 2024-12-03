#include "../common.hh"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>
#include <vector>

bool find_string(const std::string& s, size_t& i, const std::string& search) {
    i = s.find(search, i);
    if (i >= s.size()) {
        return false;
    }
    i += search.size();
    return true;
}

size_t find_first(const std::string& s, size_t& i,
                  const std::vector<std::string>& search) {
    size_t min_search_i = search.size();
    size_t min_i = s.size();
    for (size_t search_i = 0; search_i < search.size(); search_i++) {
        size_t i_ = s.find(search[search_i], i);
        if (i_ < min_i) {
            min_i = i_;
            min_search_i = search_i;
        }
    }
    i = min_i;
    if (min_search_i < search.size()) {
        i += search[min_search_i].size();
    }
    return min_search_i;
}

bool consume_unsigned(const std::string& s, size_t& i, size_t max_digits,
                      unsigned& n) {
    max_digits = std::min(max_digits, s.size() - i);
    n = 0;
    size_t j = 0;
    for (; j < max_digits && isdigit(s[i]); j++) {
        n = n * 10 + (s[i++] - '0');
    }
    return j > 0;
}

bool consume_string(const std::string& s, size_t& i,
                    const std::string& search) {
    if (s.size() - i < search.size())
        return false;
    if (memcmp(&s[i], search.data(), search.size()))
        return false;
    i += search.size();
    return true;
}

void part1(FILE* input_) {
    std::string input = read_all(input_);
    unsigned sum = 0;
    for (size_t i = 0; i < input.size();) {
        unsigned a, b;
        if (!(find_string(input, i, "mul(") &&
              consume_unsigned(input, i, 3, a) &&
              consume_string(input, i, ",") &&
              consume_unsigned(input, i, 3, b) &&
              consume_string(input, i, ")")))
            continue;
        sum += a * b;
    }
    printf("%u\n", sum);
}

void part2(FILE* input_) {
    std::string input = read_all(input_);
    bool muls_are_enabled = true;
    unsigned sum = 0;
    for (size_t i = 0; i < input.size();) {
        switch (find_first(input, i, {"mul(", "do()", "don't()"})) {
        case 0: {
            if (!muls_are_enabled)
                break;
            unsigned a, b;
            if (!(consume_unsigned(input, i, 3, a) &&
                  consume_string(input, i, ",") &&
                  consume_unsigned(input, i, 3, b) &&
                  consume_string(input, i, ")")))
                break;
            sum += a * b;
            break;
        }
        case 1: {
            muls_are_enabled = true;
            break;
        }
        case 2: {
            muls_are_enabled = false;
            break;
        }
        }
    }
    printf("%u\n", sum);
}
