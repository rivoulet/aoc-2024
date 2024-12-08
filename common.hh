#pragma once

#include <cstdio>
#include <limits>
#include <string>
#include <system_error>
#include <vector>

template <typename T> struct Vec2 {
    T x, y;

    bool operator==(const Vec2<T>& other) const noexcept {
        return x == other.x && y == other.y;
    }

    template <typename U> Vec2<T> operator+(Vec2<U> other) const noexcept {
        return {x + other.x, y + other.y};
    }

    template <typename U> Vec2<T>& operator+=(Vec2<U> other) noexcept {
        x += other.x;
        y += other.y;
        return *this;
    }

    template <typename U> Vec2<T> operator-(Vec2<U> other) const noexcept {
        return {x - other.x, y - other.y};
    }

    template <typename U> Vec2<T>& operator-=(Vec2<U> other) noexcept {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    template <typename U> Vec2<T> operator*(U other) const noexcept {
        return {x * other, y * other};
    }

    template <typename U> Vec2<T>& operator*=(U other) noexcept {
        x *= other;
        y *= other;
        return *this;
    }

    template <typename U> Vec2<T> operator/(U other) const noexcept {
        return {x / other, y / other};
    }

    template <typename U> Vec2<T>& operator/=(U other) noexcept {
        x /= other;
        y /= other;
        return *this;
    }

    template <typename U> operator Vec2<U>() const noexcept {
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
