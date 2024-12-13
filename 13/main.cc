#include "../common.hh"
#include <cstddef>
#include <vector>

struct ClawMachine {
    Vec2<size_t> a, b, prize;
};

std::vector<ClawMachine> parse_input(FILE* input) {
    auto lines = read_all_lines(input);
    std::vector<ClawMachine> result;
    for (size_t i = 0; i + 2 < lines.size(); i += 4) {
        ClawMachine machine;
        sscanf(lines[i].c_str(), "Button A: X+%zu, Y+%zu", &machine.a.x,
               &machine.a.y);
        sscanf(lines[i + 1].c_str(), "Button B: X+%zu, Y+%zu", &machine.b.x,
               &machine.b.y);
        sscanf(lines[i + 2].c_str(), "Prize: X=%zu, Y=%zu", &machine.prize.x,
               &machine.prize.y);
        result.push_back(machine);
    }
    return result;
}

size_t get_total_tokens(const std::vector<ClawMachine>& input) {
    size_t total_tokens = 0;
    for (const auto& machine : input) {
        size_t j = ((ptrdiff_t)(machine.prize.y * machine.a.x -
                                machine.prize.x * machine.a.y)) /
                   ((ptrdiff_t)(machine.b.y * machine.a.x -
                                machine.b.x * machine.a.y));
        auto i = (machine.prize.x - machine.b.x * j) / machine.a.x;
        if (machine.a * i + machine.b * j != machine.prize)
            continue;
        total_tokens += i * 3 + j;
    }
    return total_tokens;
}

void part1(FILE* input_) {
    auto input = parse_input(input_);
    size_t total_tokens = get_total_tokens(input);
    printf("%zu\n", total_tokens);
}

void part2(FILE* input_) {
    auto input = parse_input(input_);
    for (auto& machine : input) {
        machine.prize += 10000000000000;
    }
    size_t total_tokens = get_total_tokens(input);
    printf("%zu\n", total_tokens);
}
