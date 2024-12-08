#include "../common.hh"
#include <cstdio>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

template <typename T> struct Node {
    T value;
    size_t next_from, next_to;
    bool visited;
};

struct Edge {
    size_t from, to, prev_from, prev_to, next_from, next_to;
};

template <typename T> class Graph {
  private:
    std::vector<Node<T>> nodes;
    std::vector<Edge> edges;

  public:
    Node<T>& node(size_t node_i) {
        return nodes[node_i];
    }

    size_t add_node(T value) {
        size_t node_i = nodes.size();
        nodes.push_back({value, SIZE_T_MAX, SIZE_T_MAX, false});
        return node_i;
    }

    Edge& edge(size_t edge_i) {
        return edges[edge_i];
    }

    void add_edge(size_t from, size_t to) {
        auto& n_from = nodes[from];
        auto& n_to = nodes[to];
        size_t edge_i = edges.size();
        edges.push_back(
            {from, to, SIZE_T_MAX, SIZE_T_MAX, n_from.next_from, n_to.next_to});
        if (n_from.next_from < SIZE_T_MAX) {
            edges[n_from.next_from].prev_from = edge_i;
        }
        n_from.next_from = edge_i;
        if (n_to.next_to < SIZE_T_MAX) {
            edges[n_to.next_to].prev_to = edge_i;
        }
        n_to.next_to = edge_i;
    }

    void unlink_node(size_t node_i) {
        auto& node = nodes[node_i];
        for (size_t edge_i = node.next_to; edge_i < SIZE_T_MAX;) {
            auto& edge = edges[edge_i];
            auto& from_node = nodes[edge.from];
            if (edge.next_from < SIZE_T_MAX) {
                edges[edge.next_from].prev_from = edge.prev_from;
            }
            if (edge.prev_from < SIZE_T_MAX) {
                edges[edge.prev_from].next_from = edge.next_from;
            } else {
                from_node.next_from = edge.next_from;
            }
            edge_i = edge.next_to;
        }
        for (size_t edge_i = node.next_from; edge_i < SIZE_T_MAX;) {
            auto& edge = edges[edge_i];
            auto& to_node = nodes[edge.to];
            if (edge.next_to < SIZE_T_MAX) {
                edges[edge.next_to].prev_to = edge.prev_to;
            }
            if (edge.prev_to < SIZE_T_MAX) {
                edges[edge.prev_to].next_to = edge.next_to;
            } else {
                to_node.next_to = edge.next_to;
            }
            edge_i = edge.next_from;
        }
        node.next_to = SIZE_T_MAX;
        node.next_from = SIZE_T_MAX;
    }

  private:
    template <typename F> void dfs_inner(Node<T>& from_node, F f) {
        if (from_node.visited)
            return;
        from_node.visited = true;
        for (size_t edge_i = from_node.next_from; edge_i < SIZE_T_MAX;) {
            const auto& out_edge = edges[edge_i];
            auto& to_node = nodes[out_edge.to];
            dfs_inner(to_node, f);
            edge_i = out_edge.next_from;
        }
        f(from_node);
    }

  public:
    template <typename F> void dfs(F f) {
        for (auto& node : nodes) {
            node.visited = false;
        }
        for (auto& node : nodes) {
            dfs_inner(node, f);
        }
    }
};

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
        deps_filtered.dfs([&](const auto& node) {
            if (ns.count(node.value))
                ordered_ns.push_back(node.value);
        });
        sum += ordered_ns[ordered_ns.size() / 2];
    }
    printf("%u\n", sum);
}
