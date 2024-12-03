#pragma once

#include <cstdio>
#include <cstdlib>

void part1(FILE* input);
void part2(FILE* input);

#define STRINGIFY(s) #s
#define STRINGIFY_VALUE(s) STRINGIFY(s)

#define QUIT(code)                                                             \
    do {                                                                       \
        code;                                                                  \
        exit(EXIT_FAILURE);                                                    \
    } while (1);

#define ERROR(s, ...) QUIT(fprintf(stderr, s "\n", ##__VA_ARGS__))
#define PERROR(s) QUIT(perror(s))

void print_usage() {
    ERROR("Usage:\nsol <PART> [FILE]");
}

int main(int argc, const char* argv[]) {
    if (argc < 2 || argc > 3)
        print_usage();
    unsigned long part = strtoul(argv[1], NULL, 10);
    if (part < 1 || part > 2)
        print_usage();
    FILE* input;
    if (argc >= 3) {
        input = fopen(argv[2], "r");
        if (!input)
            PERROR("Couldn't read input file");
    } else {
        input = stdin;
    }
    if (part == 1) {
        part1(input);
    } else {
        part2(input);
    }
}
