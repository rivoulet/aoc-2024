#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <limits>
#include <string>
#include <system_error>
#include <unordered_set>
#include <vector>

template <typename T> struct Vec2 {
    T x, y;

    constexpr const T& min_component() const noexcept {
        return std::min(x, y);
    }

    constexpr const T& max_component() const noexcept {
        return std::max(x, y);
    }

    constexpr bool is_zero() const noexcept {
        return x == 0 && y == 0;
    }

    constexpr Vec2<T> swap_components() const noexcept {
        return {y, x};
    }

    constexpr bool operator==(const Vec2<T>& other) const noexcept {
        return x == other.x && y == other.y;
    }

    constexpr bool operator!=(const Vec2<T>& other) const noexcept {
        return x != other.x || y != other.y;
    }

    template <typename U> constexpr Vec2<T> operator+(U other) const noexcept {
        return {x + other, y + other};
    }

    template <typename U> constexpr Vec2<T>& operator+=(U other) noexcept {
        x += other;
        y += other;
        return *this;
    }

    template <typename U>
    constexpr Vec2<T> operator+(Vec2<U> other) const noexcept {
        return {x + other.x, y + other.y};
    }

    template <typename U>
    constexpr Vec2<T>& operator+=(Vec2<U> other) noexcept {
        x += other.x;
        y += other.y;
        return *this;
    }

    template <typename U> constexpr Vec2<T> operator-(U other) const noexcept {
        return {x - other, y - other};
    }

    template <typename U> constexpr Vec2<T>& operator-=(U other) noexcept {
        x -= other;
        y -= other;
        return *this;
    }

    template <typename U>
    constexpr Vec2<T> operator-(Vec2<U> other) const noexcept {
        return {x - other.x, y - other.y};
    }

    template <typename U>
    constexpr Vec2<T>& operator-=(Vec2<U> other) noexcept {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    template <typename U>
    constexpr Vec2<T> operator*(Vec2<U> other) const noexcept {
        return {x * other.x, y * other.y};
    }

    template <typename U>
    constexpr Vec2<T>& operator*=(Vec2<U> other) noexcept {
        x *= other.x;
        y *= other.y;
        return *this;
    }

    template <typename U> constexpr Vec2<T> operator*(U other) const noexcept {
        return {x * other, y * other};
    }

    template <typename U> constexpr Vec2<T>& operator*=(U other) noexcept {
        x *= other;
        y *= other;
        return *this;
    }

    template <typename U>
    constexpr Vec2<T> operator/(Vec2<U> other) const noexcept {
        return {x / other.x, y / other.y};
    }

    template <typename U>
    constexpr Vec2<T>& operator/=(Vec2<U> other) noexcept {
        x /= other.x;
        y /= other.y;
        return *this;
    }

    template <typename U> constexpr Vec2<T> operator/(U other) const noexcept {
        return {x / other, y / other};
    }

    template <typename U> constexpr Vec2<T>& operator/=(U other) noexcept {
        x /= other;
        y /= other;
        return *this;
    }

    template <typename U>
    constexpr Vec2<T> operator%(Vec2<U> other) const noexcept {
        return {x % other.x, y % other.y};
    }

    template <typename U>
    constexpr Vec2<T>& operator%=(Vec2<U> other) noexcept {
        x %= other.x;
        y %= other.y;
        return *this;
    }

    template <typename U> constexpr Vec2<T> operator%(U other) const noexcept {
        return {x % other, y % other};
    }

    template <typename U> constexpr Vec2<T>& operator%=(U other) noexcept {
        x %= other;
        y %= other;
        return *this;
    }

    template <typename U> constexpr operator Vec2<U>() const noexcept {
        return {(U)x, (U)y};
    }
};

constexpr auto SIZE_T_BITS = std::numeric_limits<size_t>::digits;

template <typename T> T rotl(T x, unsigned s) noexcept {
    return (x << s) | (x >> (SIZE_T_BITS - s));
}

template <typename T> struct std::hash<Vec2<T>> {
    size_t operator()(const Vec2<T>& v) const noexcept {
        return rotl(std::hash<T>{}(v.x), SIZE_T_BITS >> 1) ^
               std::hash<T>{}(v.y);
    }
};

template <typename T, typename U> struct std::hash<std::pair<T, U>> {
    size_t operator()(const std::pair<T, U>& p) const noexcept {
        return rotl(std::hash<T>{}(p.first), SIZE_T_BITS >> 1) ^
               std::hash<U>{}(p.second);
    }
};

template <typename T, size_t _size> struct std::hash<std::array<T, _size>> {
    size_t operator()(const std::array<T, _size>& a) const noexcept {
        size_t hash = 0;
        for (const auto& e : a) {
            hash = rotl(hash, 1) ^ std::hash<T>{}(e);
        }
        return hash;
    }
};

inline int read_char(FILE* f) {
    int c = fgetc(f);
    if (c == EOF) {
        int err = ferror(f);
        if (err) {
            throw std::system_error(err, std::system_category());
        }
    }
    return c;
}

constexpr size_t BUF_START_SIZE = 512;

inline std::string read_all(FILE* f) {
    size_t buf_size = BUF_START_SIZE;
    char* buf = (char*)malloc(buf_size);
    for (size_t i = 0;;) {
        if (i + 1 >= buf_size) {
            buf_size <<= 1;
            buf = (char*)realloc(buf, buf_size);
        }
        size_t read_bytes = fread(buf + i, 1, buf_size - i, f);
        i += read_bytes;
        int err = ferror(f);
        if (err) {
            throw std::system_error(err, std::system_category());
        }
        if (feof(f))
            break;
    }
    std::string result(buf);
    free(buf);
    return result;
}

inline std::string read_line(FILE* f, bool remove_newline = false) {
    size_t buf_size = BUF_START_SIZE;
    char* buf = (char*)malloc(buf_size);
    size_t i = 0;
    do {
        if (i + 1 >= buf_size) {
            buf_size <<= 1;
            buf = (char*)realloc(buf, buf_size);
        }
        if (fgets(buf + i, buf_size - i, f)) {
            i += strlen(buf + i);
            if (buf[i - 1] == '\n')
                break;
        } else {
            int err = ferror(f);
            if (err) {
                throw std::system_error(err, std::system_category());
            }
            // EOF
            buf[i] = 0;
        }
    } while (!feof(f));
    std::string result = buf;
    free(buf);
    if (remove_newline && !result.empty() && result.back() == '\n')
        result.pop_back();
    return result;
}

inline std::vector<std::string> read_all_lines(FILE* f) {
    std::vector<std::string> result;
    size_t buf_size = BUF_START_SIZE;
    char* buf = (char*)malloc(buf_size);
    size_t i = 0;
    do {
        if (i + 1 >= buf_size) {
            buf_size <<= 1;
            buf = (char*)realloc(buf, buf_size);
        }
        if (fgets(buf + i, buf_size - i, f)) {
            i += strlen(buf + i);
            if (!feof(f) && buf[i - 1] != '\n')
                continue;
        } else {
            int err = ferror(f);
            if (err) {
                throw std::system_error(err, std::system_category());
            }
            // EOF
            buf[i] = 0;
        }
        result.push_back(buf);
        i = 0;
    } while (!feof(f));
    free(buf);
    return result;
}

template <typename T> struct Grid {
    std::vector<T> contents;
    Vec2<size_t> size;

    Grid(std::vector<T> contents, Vec2<size_t> size) noexcept
        : contents(contents), size(size) {}

    Grid(Vec2<size_t> size) noexcept : contents(size.x * size.y), size(size) {}

    template <typename U>
    Grid(Vec2<size_t> size, U fill_value) noexcept
        : contents(size.x * size.y, fill_value), size(size) {}

    Grid(FILE* input)
        : Grid(input, [](char c, Vec2<size_t> pos) { return c; }) {}

    template <typename F> Grid(FILE* input, F parse) : size({0, 0}) {
        size_t cur_width = 0;
        for (int c; (c = read_char(input)) != EOF;) {
            if (c == '\n') {
                if (cur_width == 0)
                    break;
                if (size.x) {
                    assert(cur_width == size.x);
                } else {
                    size.x = cur_width;
                }
                cur_width = 0;
                size.y++;
            } else {
                contents.push_back(parse(c, Vec2<size_t>{cur_width, size.y}));
                cur_width++;
            }
        }
    }

    size_t pos_to_i(Vec2<size_t> pos) const noexcept {
        return pos.y * size.x + pos.x;
    }

    bool pos_is_inside(Vec2<size_t> pos) const noexcept {
        return pos.x < size.x && pos.y < size.y;
    }

    template <typename U> void fill(U fill_value) noexcept {
        std::fill(contents.begin(), contents.end(), fill_value);
    }

    Vec2<size_t> find(const T& value) const noexcept {
        for (size_t y = 0; y < size.y; y++) {
            for (size_t x = 0; x < size.x; x++) {
                Vec2<size_t> pos{x, y};
                if ((*this)[pos] == value)
                    return pos;
            }
        }
        return size;
    }

    typename std::vector<T>::const_reference
    operator[](Vec2<size_t> pos) const noexcept {
        return contents[pos_to_i(pos)];
    }

    typename std::vector<T>::reference operator[](Vec2<size_t> pos) noexcept {
        return contents[pos_to_i(pos)];
    }
};

enum class GridDir {
    Right,
    Left,
    Down,
    Up,
};

constexpr std::array<GridDir, 4> grid_dirs{GridDir::Right, GridDir::Left,
                                           GridDir::Down, GridDir::Up};

constexpr std::array<Vec2<ptrdiff_t>, 4> grid_dir_offsets{
    Vec2<ptrdiff_t>{1, 0},
    {-1, 0},
    {0, 1},
    {0, -1},
};

constexpr Vec2<ptrdiff_t> grid_dir_offset(GridDir dir) noexcept {
    return grid_dir_offsets[(size_t)dir];
}

constexpr uint8_t grid_dir_mask(GridDir dir) noexcept {
    return 1 << (size_t)dir;
}

constexpr bool grid_dir_is_horizontal(GridDir dir) noexcept {
    switch (dir) {
    case GridDir::Right:
    case GridDir::Left:
        return true;
    case GridDir::Down:
    case GridDir::Up:
        return false;
    }
}

constexpr GridDir rotate_grid_dir_clockwise(GridDir dir) noexcept {
    switch (dir) {
    case GridDir::Up:
        return GridDir::Right;
    case GridDir::Right:
        return GridDir::Down;
    case GridDir::Down:
        return GridDir::Left;
    case GridDir::Left:
        return GridDir::Up;
    }
}

constexpr GridDir rotate_grid_dir_counterclockwise(GridDir dir) noexcept {
    switch (dir) {
    case GridDir::Up:
        return GridDir::Left;
    case GridDir::Left:
        return GridDir::Down;
    case GridDir::Down:
        return GridDir::Right;
    case GridDir::Right:
        return GridDir::Up;
    }
}

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
        nodes.push_back({value, SIZE_MAX, SIZE_MAX, false});
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
            {from, to, SIZE_MAX, SIZE_MAX, n_from.next_from, n_to.next_to});
        if (n_from.next_from < SIZE_MAX) {
            edges[n_from.next_from].prev_from = edge_i;
        }
        n_from.next_from = edge_i;
        if (n_to.next_to < SIZE_MAX) {
            edges[n_to.next_to].prev_to = edge_i;
        }
        n_to.next_to = edge_i;
    }

    bool nodes_are_connected(size_t from, size_t to) {
        for (size_t edge_i = nodes[from].next_from; edge_i != SIZE_MAX;) {
            const auto& edge = edges[edge_i];
            if (edges[edge_i].to == to)
                return true;
            edge_i = edge.next_from;
        }
        return false;
    }

  private:
    template <typename F>
    void dfs_inner(size_t from, F f, std::vector<bool>& visited) {
        if (visited[from])
            return;
        visited[from] = true;
        for (size_t edge_i = nodes[from].next_from; edge_i != SIZE_MAX;) {
            const auto& edge = edges[edge_i];
            dfs_inner(edge.to, f, visited);
            edge_i = edge.next_from;
        }
        f(from);
    }

  public:
    template <typename F> void dfs(F f) {
        std::vector<bool> visited(nodes.size());
        for (auto& node : nodes) {
            node.visited = false;
        }
        for (size_t from = 0; from < nodes.size(); from++) {
            dfs_inner(from, f, visited);
        }
    }

  private:
    void maximal_cliques_inner(std::vector<std::vector<size_t>>& result,
                               std::vector<size_t>& yes,
                               std::unordered_set<size_t> maybe,
                               std::unordered_set<size_t> no) {
        if (maybe.empty() && no.empty()) {
            result.push_back(yes);
            return;
        }
        while (!maybe.empty()) {
            auto n = *maybe.begin();
            maybe.erase(n);

            std::unordered_set<size_t> neighbors;
            for (size_t edge_i = nodes[n].next_from; edge_i != SIZE_MAX;) {
                const auto& edge = edges[edge_i];
                neighbors.emplace(edge.to);
                edge_i = edge.next_from;
            }

            std::unordered_set<size_t> n_maybe, n_no;
            for (const auto m : maybe) {
                if (neighbors.count(m))
                    n_maybe.emplace(m);
            }
            for (const auto m : no) {
                if (neighbors.count(m))
                    n_no.emplace(m);
            }

            yes.push_back(n);
            maximal_cliques_inner(result, yes, n_maybe, n_no);
            yes.pop_back();

            maybe.erase(n);
            no.emplace(n);
        }
    }

  public:
    std::vector<std::vector<size_t>> maximal_cliques() {
        std::vector<std::vector<size_t>> result;
        std::vector<size_t> yes;
        std::unordered_set<size_t> maybe;
        for (size_t i = 0; i < nodes.size(); i++) {
            maybe.emplace(i);
        }
        maximal_cliques_inner(result, yes, maybe, {});
        return result;
    }

    void unlink_node(size_t node_i) {
        auto& node = nodes[node_i];
        for (size_t edge_i = node.next_to; edge_i < SIZE_MAX;) {
            auto& edge = edges[edge_i];
            auto& n_from = nodes[edge.from];
            if (edge.next_from < SIZE_MAX) {
                edges[edge.next_from].prev_from = edge.prev_from;
            }
            if (edge.prev_from < SIZE_MAX) {
                edges[edge.prev_from].next_from = edge.next_from;
            } else {
                n_from.next_from = edge.next_from;
            }
            edge_i = edge.next_to;
        }
        for (size_t edge_i = node.next_from; edge_i < SIZE_MAX;) {
            auto& edge = edges[edge_i];
            auto& n_to = nodes[edge.to];
            if (edge.next_to < SIZE_MAX) {
                edges[edge.next_to].prev_to = edge.prev_to;
            }
            if (edge.prev_to < SIZE_MAX) {
                edges[edge.prev_to].next_to = edge.next_to;
            } else {
                n_to.next_to = edge.next_to;
            }
            edge_i = edge.next_from;
        }
        node.next_to = SIZE_MAX;
        node.next_from = SIZE_MAX;
    }
};
