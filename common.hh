#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <limits>
#include <string>
#include <system_error>
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
    std::size_t operator()(const Vec2<T>& v) const noexcept {
        return rotl(std::hash<T>{}(v.x), SIZE_T_BITS >> 1) |
               std::hash<T>{}(v.y);
    }
};

template <typename T, typename U> struct std::hash<std::pair<T, U>> {
    std::size_t operator()(const std::pair<T, U>& p) const noexcept {
        return rotl(std::hash<T>{}(p.first), SIZE_T_BITS >> 1) |
               std::hash<U>{}(p.second);
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
        if (i == buf_size) {
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

inline std::vector<std::string> read_all_lines(FILE* f) {
    std::vector<std::string> result;
    size_t buf_size = BUF_START_SIZE;
    char* buf = (char*)malloc(buf_size);
    size_t i = 0;
    do {
        if (i == buf_size) {
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

    size_t pos_to_i(Vec2<size_t> pos) {
        return pos.y * size.x + pos.x;
    }

    bool pos_is_inside(Vec2<size_t> pos) {
        return pos.x < size.x && pos.y < size.y;
    }

    template <typename U> void fill(U fill_value) {
        std::fill(contents.begin(), contents.end(), fill_value);
    }

    typename std::vector<T>::reference operator[](Vec2<size_t> pos) {
        return contents[pos_to_i(pos)];
    }
};

enum class GridDir {
    Right,
    Left,
    Down,
    Up,
};

constexpr std::array<Vec2<ptrdiff_t>, 4> grid_dir_offsets{
    Vec2<ptrdiff_t>{1, 0},
    {-1, 0},
    {0, 1},
    {0, -1},
};

constexpr Vec2<ptrdiff_t> grid_dir_offset(GridDir dir) noexcept {
    return grid_dir_offsets[(size_t)dir];
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
