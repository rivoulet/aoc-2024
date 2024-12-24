#include "../common.hh"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>

std::optional<unsigned> parse_unsigned(const std::string& s, size_t i) {
    if (i >= s.size())
        return {};
    char* end_ptr;
    auto value = strtoul(s.c_str() + 1, &end_ptr, 10);
    if (end_ptr != s.c_str() + s.size())
        return {};
    return value;
}

using GateName = std::string;

enum class Op {
    And,
    Or,
    Xor,
};

bool get_op_result(Op op, bool a, bool b) {
    switch (op) {
    case Op::And:
        return a & b;
    case Op::Or:
        return a | b;
    case Op::Xor:
        return a ^ b;
    }
}

struct UnknownGate {
    Op op;
    std::array<GateName, 2> deps;
};

using GateCache = std::unordered_map<GateName, std::optional<bool>>;

struct Input {
    size_t x, y;
    unsigned input_bits;
    std::unordered_map<GateName, UnknownGate> gates;
    std::vector<GateName> z_names;

    Input(FILE* input) : x(0), y(0) {
        unsigned x_bits = 0, y_bits = 0;
        for (std::string line; !(line = read_line(input, true)).empty();) {
            size_t colon_i = line.find(':');
            assert(colon_i + 3 == line.size());
            GateName name = line.substr(0, colon_i);
            assert(name.size() == 3);
            bool value = line[colon_i + 2] == '1';
            auto bit = parse_unsigned(name, 1);
            assert(bit.has_value());
            if (name[0] == 'x') {
                x |= (size_t)value << bit.value();
                x_bits = std::max(x_bits, bit.value() + 1);
                continue;
            } else if (name[0] == 'y') {
                y |= (size_t)value << bit.value();
                y_bits = std::max(y_bits, bit.value() + 1);
                continue;
            }
            assert(false);
        }
        assert(x_bits == y_bits);
        input_bits = x_bits;
        for (std::string line; !(line = read_line(input, true)).empty();) {
            size_t first_end_i = line.find(' ');
            size_t op_end_i = line.find(' ', first_end_i + 1);
            size_t second_end_i = line.find(' ', op_end_i + 1);
            size_t arrow_end_i = line.find(' ', second_end_i + 1);
            assert(std::max({first_end_i, op_end_i, second_end_i,
                             arrow_end_i}) < line.size());
            GateName a = line.substr(0, first_end_i),
                     b = line.substr(op_end_i + 1,
                                     second_end_i - (op_end_i + 1)),
                     out = line.substr(arrow_end_i + 1);
            assert(a.size() == 3 && b.size() == 3 && out.size() == 3);
            auto op_name = std::string_view(line).substr(
                first_end_i + 1, op_end_i - (first_end_i + 1));
            Op op;
            if (!op_name.compare("AND")) {
                op = Op::And;
            } else if (!op_name.compare("OR")) {
                op = Op::Or;
            } else if (!op_name.compare("XOR")) {
                op = Op::Xor;
            } else {
                assert(false);
            }
            gates.emplace(out, UnknownGate{op, {a, b}});
        }
        for (size_t i = 0; i < input_bits + 1; i++) {
            std::string name = "z";
            name.push_back('0' + i / 10);
            name.push_back('0' + i % 10);
            z_names.push_back(name);
        }
    }

    std::optional<bool> calc_gate_value(const GateName& name,
                                        GateCache& cache) const noexcept {
        if (name[0] == 'x') {
            auto bit = parse_unsigned(name, 1);
            assert(bit.has_value());
            return x & ((size_t)1 << bit.value());
        } else if (name[0] == 'y') {
            auto bit = parse_unsigned(name, 1);
            assert(bit.has_value());
            return y & ((size_t)1 << bit.value());
        }
        assert(gates.count(name));
        if (cache.count(name)) {
            return cache[name];
        }
        cache[name] = {};
        const auto& gate = gates.at(name);
        auto a = calc_gate_value(gate.deps[0], cache),
             b = calc_gate_value(gate.deps[1], cache);
        if (!a.has_value() || !b.has_value())
            return {};
        auto value = get_op_result(gate.op, a.value(), b.value());
        return cache[name] = value;
    }

    std::optional<size_t> calc_z(GateCache& cache) const noexcept {
        size_t z = 0;
        for (size_t z_bit = 0; z_bit < input_bits + 1; z_bit++) {
            auto bit_value = calc_gate_value(z_names[z_bit], cache);
            if (!bit_value.has_value())
                return {};
            z |= (size_t)bit_value.value() << z_bit;
        }
        return z;
    }
};

void part1(FILE* input_) {
    Input input(input_);
    GateCache cache;
    auto z = input.calc_z(cache);
    assert(z.has_value());
    printf("%zu\n", z.value());
}

bool check_z_bit(Input& input, size_t z_bit, GateCache& cache) {
    auto input_mask = ((size_t)1 << input.input_bits) - 1;
    for (size_t x_bits_value = 0; x_bits_value <= 3; x_bits_value++) {
        input.x = (x_bits_value << z_bit >> 1) & input_mask;
        for (size_t y_bits_value = 0; y_bits_value <= 3; y_bits_value++) {
            input.y = (y_bits_value << z_bit >> 1) & input_mask;
            bool wanted = ((input.x + input.y) >> z_bit) & 1;
            cache.clear();
            auto z = input.calc_z(cache);
            if (!z.has_value())
                return false;
            bool current = (z.value() >> z_bit) & 1;
            if (current != wanted)
                return false;
        }
    }
    return true;
}

template <typename F>
void visit_gates(const Input& input, const GateName& name, F f) {
    if (!input.gates.count(name))
        return;
    f(name);
    const auto& gate = input.gates.at(name);
    visit_gates(input, gate.deps[0], f);
    visit_gates(input, gate.deps[1], f);
}

std::array<GateName, 2> correct_bit(Input& input, size_t z_bit,
                                    const std::unordered_set<GateName>& correct,
                                    const std::unordered_set<GateName>& used,
                                    GateCache& cache) {
    for (const auto& a_name : used) {
        auto& a_gate = input.gates[a_name];
        for (auto& [b_name, b_gate] : input.gates) {
            if (correct.count(b_name) || b_name == a_name)
                continue;
            std::swap(a_gate, b_gate);
            if (check_z_bit(input, z_bit, cache)) {
                return {a_name, b_name};
            }
            std::swap(a_gate, b_gate);
        }
    }
    assert(false);
}

void part2(FILE* input_) {
    Input input(input_);
    GateCache cache;
    std::unordered_set<GateName> correct, used;
    std::vector<GateName> swapped;
    for (size_t z_bit = 0; z_bit < input.input_bits + 1; z_bit++) {
        auto is_correct = check_z_bit(input, z_bit, cache);
        used.clear();
        visit_gates(input, input.z_names[z_bit],
                    [&](const GateName& name) { used.emplace(name); });
        if (!is_correct) {
            auto [a_name, b_name] =
                correct_bit(input, z_bit, correct, used, cache);
            swapped.push_back(a_name);
            swapped.push_back(b_name);
            used.clear();
            visit_gates(input, input.z_names[z_bit],
                        [&](const GateName& name) { used.emplace(name); });
        }
        for (const auto& name : used) {
            correct.emplace(name);
        }
    }
    std::sort(swapped.begin(), swapped.end());
    for (size_t i = 0; i < swapped.size(); i++) {
        if (i)
            printf(",");
        printf("%s", swapped[i].c_str());
    }
    printf("\n");
}
