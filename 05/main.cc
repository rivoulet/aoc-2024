#include "../common.hh"
#include <cstdio>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

void part1(FILE* input_) {
    const std::vector<std::string> input = read_all_lines(input_);

    std::vector<std::pair<unsigned, unsigned>> rules;

    size_t i = 0;
    for (; i < input.size(); i++) {
        unsigned a, b;
        if (sscanf(input[i].c_str(), "%u|%u", &a, &b) < 2)
            break;
        rules.push_back({a, b});
    }

    i++;

    unsigned sum = 0;
    std::vector<unsigned> ns;
    std::unordered_map<unsigned, size_t> ns_indices;
    for (; i < input.size(); i++) {
        if (input[i].empty())
            continue;
        ns.clear();
        ns_indices.clear();
        {
            unsigned n, read_chars;
            for (size_t j = 0;
                 sscanf(input[i].c_str() + j, "%d,%n", &n, &read_chars) == 1;
                 j += read_chars) {
                ns_indices.emplace(n, ns.size());
                ns.push_back(n);
            }
        }
        for (const auto& [a, b] : rules) {
            if (ns_indices.count(a) && ns_indices.count(b)) {
                if (ns_indices[a] > ns_indices[b])
                    goto failed;
            }
        }
        sum += ns[ns.size() / 2];
    failed:;
    }
    printf("%u\n", sum);
}

void part2(FILE* input_) {
    const std::vector<std::string> input = read_all_lines(input_);

    std::vector<std::pair<unsigned, unsigned>> rules;
    Graph<unsigned> deps;
    std::unordered_map<unsigned, size_t> n_to_deps_node_is;

    size_t i = 0;
    for (; i < input.size(); i++) {
        unsigned a, b;
        if (sscanf(input[i].c_str(), "%u|%u", &a, &b) < 2)
            break;
        rules.push_back({a, b});
        if (!n_to_deps_node_is.count(a))
            n_to_deps_node_is.emplace(a, deps.add_node(a));
        if (!n_to_deps_node_is.count(b))
            n_to_deps_node_is.emplace(b, deps.add_node(b));
        deps.add_edge(n_to_deps_node_is[b], n_to_deps_node_is[a]);
    }

    i++;

    unsigned sum = 0;
    std::unordered_set<unsigned> ns;
    std::unordered_map<unsigned, size_t> n_to_is;
    std::vector<unsigned> ordered_ns;
    for (; i < input.size(); i++) {
        if (input[i].empty())
            continue;
        ns.clear();
        n_to_is.clear();
        {
            unsigned n, read_chars;
            for (size_t j = 0;
                 sscanf(input[i].c_str() + j, "%d,%n", &n, &read_chars) == 1;
                 j += read_chars) {
                n_to_is.emplace(n, ns.size());
                ns.insert(n);
            }
        }
        for (const auto& [a, b] : rules) {
            if (n_to_is.count(a) && n_to_is.count(b)) {
                if (n_to_is[a] > n_to_is[b])
                    goto failed;
            }
        }
        continue;
    failed:;
        Graph deps_filtered = deps;
        for (const auto& n_to_deps_node_i : n_to_deps_node_is) {
            if (!ns.count(n_to_deps_node_i.first))
                deps_filtered.unlink_node(n_to_deps_node_i.second);
        }
        ordered_ns.clear();
        deps_filtered.dfs([&](size_t node_i) {
            const auto& node = deps_filtered.node(node_i);
            if (ns.count(node.value))
                ordered_ns.push_back(node.value);
        });
        sum += ordered_ns[ordered_ns.size() / 2];
    }
    printf("%u\n", sum);
}
