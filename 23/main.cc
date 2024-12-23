#include "../common.hh"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <map>

using ComputerName = std::array<char, 2>;

struct CompareName {
    bool operator()(const ComputerName& lhs, const ComputerName& rhs) const {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                            rhs.end());
    }
};

struct Input {
    std::map<ComputerName, size_t, CompareName> computers;
    Graph<ComputerName> graph;

    Input(FILE* input) {
        for (std::string line; !(line = read_line(input)).empty();) {
            assert(line.size() >= 5 && line[2] == '-');
            ComputerName a_name{line[0], line[1]};
            ComputerName b_name{line[3], line[4]};
            if (!computers.count(a_name))
                computers[a_name] = graph.add_node(a_name);
            if (!computers.count(b_name))
                computers[b_name] = graph.add_node(b_name);
            graph.add_edge(computers[a_name], computers[b_name]);
            graph.add_edge(computers[b_name], computers[a_name]);
        }
    }
};

void part1(FILE* input_) {
    Input input(input_);
    size_t count = 0;
    for (const auto& [a_name, a_i] : input.computers) {
        for (auto b_it = input.computers.upper_bound(a_name);
             b_it != input.computers.end(); b_it++) {
            const auto& [b_name, b_i] = *b_it;
            for (auto c_it = input.computers.upper_bound(b_name);
                 c_it != input.computers.end(); c_it++) {
                const auto& [c_name, c_i] = *c_it;
                if (!input.graph.nodes_are_connected(a_i, b_i) ||
                    !input.graph.nodes_are_connected(b_i, c_i) ||
                    !input.graph.nodes_are_connected(c_i, a_i)) {
                    continue;
                }
                if (a_name[0] != 't' && b_name[0] != 't' && c_name[0] != 't')
                    continue;
                count++;
            }
        }
    }
    printf("%zu\n", count);
}

void part2(FILE* input_) {
    Input input(input_);
    auto cliques = input.graph.maximal_cliques();
    std::vector<size_t> max_clique;
    for (const auto& clique : cliques) {
        if (clique.size() > max_clique.size())
            max_clique = clique;
    }
    std::sort(max_clique.begin(), max_clique.end(), [&](size_t a, size_t b) {
        const auto &n_a = input.graph.node(a), &n_b = input.graph.node(b);
        return CompareName{}(n_a.value, n_b.value);
    });
    for (size_t i = 0; i < max_clique.size(); i++) {
        if (i)
            printf(",");
        printf("%.2s", input.graph.node(max_clique[i]).value.data());
    }
    printf("\n");
}
