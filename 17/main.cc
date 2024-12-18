#include "../common.hh"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <vector>

struct Input {
    size_t a, b, c;
    std::vector<uint8_t> program;

    Input(FILE* input) {
        fscanf(input,
               "Register A: %zu\nRegister B: %zu\nRegister C: %zu\n\nProgram: ",
               &a, &b, &c);
        do {
            program.push_back(read_char(input) - '0');
        } while (read_char(input) == ',');
    }
};

struct State {
    size_t a, b, c, pc;
    std::vector<uint8_t> output;

    State(Input input) : a(input.a), b(input.b), c(input.c), pc(0) {}

    size_t get_combo_op_value(uint8_t op) {
        switch (op) {
        case 4:
            return a;
        case 5:
            return b;
        case 6:
            return c;
        }
        return op;
    }

    void run(const std::vector<uint8_t>& program) {
        while (pc < program.size()) {
            uint8_t instr = program[pc++];
            uint8_t op = program[pc++];
            switch (instr) {
            case 0: {
                // adv
                a >>= get_combo_op_value(op);
                break;
            }
            case 1: {
                // bxl
                b ^= op;
                break;
            }
            case 2: {
                // bst
                b = get_combo_op_value(op) & 7;
                break;
            }
            case 3: {
                // jnz
                if (a != 0) {
                    pc = op;
                }
                break;
            }
            case 4: {
                // bxc
                b ^= c;
                break;
            }
            case 5: {
                // out
                output.push_back(get_combo_op_value(op) & 7);
                break;
            }
            case 6: {
                // bdv
                b = a >> get_combo_op_value(op);
                break;
            }
            case 7: {
                // cdv
                c = a >> get_combo_op_value(op);
                break;
            }
            }
        }
    }
};

struct ReversingState {
    size_t a, b, c, pc, output_i, a_mask, b_mask, c_mask;

    ReversingState(Input input)
        : a(), b(), c(), pc(input.program.size()),
          output_i(input.program.size()), a_mask(), b_mask(), c_mask() {}

    std::pair<size_t*, size_t*> get_combo_op(uint8_t op) {
        switch (op) {
        case 4: {
            return {&a, &a_mask};
        }
        case 5: {
            return {&b, &b_mask};
        }
        case 6: {
            return {&c, &c_mask};
        }
        }
        assert(false);
    }

    template <typename F>
    std::optional<size_t> recurse(const std::vector<uint8_t>& program,
                                  uint8_t op, size_t mask, F f) {
        auto [reg, reg_mask] = get_combo_op(op);
        size_t missing_mask = mask & ~*reg_mask;
        if (missing_mask) {
            for (uint8_t i = 0;; i++) {
                if (!(missing_mask & (1 << i)))
                    continue;

                ReversingState s1(*this);
                auto [reg_1, reg_mask_1] = s1.get_combo_op(op);
                *reg_1 &= ~(1 << i);
                *reg_mask_1 |= 1 << i;
                auto a1 = s1.recurse(program, op, mask, f);

                ReversingState s2(*this);
                auto [reg_2, reg_mask_2] = s2.get_combo_op(op);
                *reg_2 |= 1 << i;
                *reg_mask_2 |= 1 << i;
                auto a2 = s2.recurse(program, op, mask, f);

                if (!(a1.has_value() || a2.has_value()))
                    return {};
                return std::min(a1.value_or(SIZE_MAX), a2.value_or(SIZE_MAX));
            }
            assert(false);
        } else {
            if (!f(*this))
                return {};
            return run(program);
        }
    }

    std::optional<size_t> run(const std::vector<uint8_t>& program) {
        for (;;) {
            uint8_t op = program[--pc];
            uint8_t instr = program[--pc];
            switch (instr) {
            case 0: {
                // adv
                auto apply = [](ReversingState& state, uint8_t shift) {
                    state.a <<= shift;
                    state.a_mask <<= shift;
                };
                if (op < 4) {
                    apply(*this, op);
                } else {
                    return recurse(program, op, SIZE_T_BITS - 1,
                                   [=](ReversingState& state) {
                                       auto [reg, reg_mask] =
                                           state.get_combo_op(op);
                                       apply(state, *reg & (SIZE_T_BITS - 1));
                                       return true;
                                   });
                }
                break;
            }
            case 1: {
                // bxl
                b = (b ^ op) & b_mask;
                break;
            }
            case 2: {
                // bst
                if (op < 4) {
                    if ((b & b_mask) != (op & b_mask))
                        return {};
                } else {
                    if ((b & b_mask & ~7) != 0)
                        return {};
                    auto [reg, reg_mask] = get_combo_op(op);
                    if ((*reg & *reg_mask & b_mask & 7) !=
                        (b & *reg_mask & b_mask & 7))
                        return {};
                    *reg = (*reg & ~7) | (b & b_mask & 7);
                    *reg_mask |= b_mask & 7;
                }
                b = 0;
                b_mask = 0;
                break;
            }
            case 3: {
                // jnz
                if (output_i == program.size()) {
                    a = 0;
                    a_mask = SIZE_MAX;
                }
                break;
            }
            case 4: {
                // bxc
                return recurse(program, 6, b_mask, [](ReversingState& state) {
                    state.b ^= state.c;
                    return true;
                });
            }
            case 5: {
                // out
                uint8_t value = program[--output_i];
                if (op < 4) {
                    if (value != op)
                        return {};
                } else {
                    auto [reg, reg_mask] = get_combo_op(op);
                    if ((*reg & *reg_mask & 7) != (value & *reg_mask))
                        return {};
                    *reg = (*reg & !7) | value;
                    *reg_mask |= 7;
                }
                break;
            }
            case 6: {
                // bdv
                auto apply = [](ReversingState& state, uint8_t shift) {
                    auto value = state.b << shift;
                    auto mask = state.b_mask << shift;
                    if ((state.a & state.a_mask & mask) !=
                        (value & state.a_mask & mask))
                        return false;
                    state.a = (state.a & ~mask) | (value & mask);
                    state.a_mask |= mask;
                    state.b = 0;
                    state.b_mask = 0;
                    return true;
                };
                if (op < 4) {
                    if (!apply(*this, op))
                        return {};
                } else {
                    return recurse(
                        program, op, SIZE_T_BITS - 1,
                        [=](ReversingState& state) {
                            auto [reg, reg_mask] = state.get_combo_op(op);
                            return apply(state, *reg & (SIZE_T_BITS - 1));
                        });
                }
                break;
            }
            case 7: {
                // cdv
                auto apply = [](ReversingState& state, uint8_t shift) {
                    auto value = state.c << shift;
                    auto mask = state.c_mask << shift;
                    if ((state.a & state.a_mask & mask) !=
                        (value & state.a_mask & mask))
                        return false;
                    state.a = (state.a & ~mask) | (value & mask);
                    state.a_mask |= mask;
                    state.c = 0;
                    state.c_mask = 0;
                    return true;
                };
                if (op < 4) {
                    if (!apply(*this, op))
                        return {};
                } else {
                    return recurse(
                        program, op, SIZE_T_BITS - 1,
                        [=](ReversingState& state) {
                            auto [reg, reg_mask] = state.get_combo_op(op);
                            return apply(state, *reg & (SIZE_T_BITS - 1));
                        });
                }
            }
            }
            if (pc == 0) {
                if (output_i == 0)
                    break;
                pc = program.size();
            }
        }
        assert(a_mask == SIZE_MAX);
        return a;
    }
};

void part1(FILE* input_) {
    Input input(input_);
    State state(input);
    state.run(input.program);
    for (size_t i = 0; i < state.output.size(); i++) {
        if (i)
            printf(",");
        printf("%u", state.output[i]);
    }
    printf("\n");
}

void part2(FILE* input_) {
    // NOTE: The resolution algorithm might be input-specific
    Input input(input_);
    size_t min = SIZE_MAX;
    for (size_t b = 0; b < 8; b++) {
        for (size_t c = 0; c < 8; c++) {
            ReversingState reversing_state(input);
            reversing_state.b = b;
            reversing_state.b_mask = 7;
            reversing_state.c = c;
            reversing_state.c_mask = 7;
            min = std::min(
                min, reversing_state.run(input.program).value_or(SIZE_MAX));
        }
    }
    printf("%zu\n", min);
}
