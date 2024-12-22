#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <vector>

void part1(FILE* input) {
    std::vector<unsigned> l, r;
    for (;;) {
        unsigned l_element, r_element;
        if (fscanf(input, "%u %u", &l_element, &r_element) <= 0)
            break;
        l.push_back(l_element);
        r.push_back(r_element);
    }
    std::sort(l.begin(), l.end());
    std::sort(r.begin(), r.end());
    unsigned result = 0;
    for (size_t i = 0; i < l.size(); i++) {
        result += l[i] > r[i] ? l[i] - r[i] : r[i] - l[i];
    }
    printf("%d\n", result);
}

void part2(FILE* input) {
    std::vector<unsigned> l, r;
    for (;;) {
        unsigned l_element, r_element;
        if (fscanf(input, "%u %u", &l_element, &r_element) <= 0)
            break;
        l.push_back(l_element);
        r.push_back(r_element);
    }
    std::sort(l.begin(), l.end());
    std::sort(r.begin(), r.end());
    unsigned similarity = 0;
    for (size_t i = 0, j = 0; i < l.size();) {
        unsigned e = l[i];
        unsigned e_similarity = 0;
        while (r[j] < e && j < l.size())
            j++;
        while (r[j] == e && j < l.size()) {
            e_similarity++;
            j++;
        }
        e_similarity *= e;
        while (l[i] == e && i < l.size()) {
            similarity += e_similarity;
            i++;
        }
    }
    printf("%d\n", similarity);
}
