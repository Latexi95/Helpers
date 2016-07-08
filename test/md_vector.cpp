#include <iostream>
#include "md_vector.h"

int main() {
    md_vector<std::string, 2, md_vector_order::row_major> vec;
    vec.resize({3, 2});

    int i = 0;
    for (int y = 0; y < vec.size(0); ++y) {
        for (int x = 0; x < vec.size(1); ++x) {
            vec[y][x]  = std::to_string(i) + ": " + std::to_string(y) + ", " + std::to_string(x);
            ++i;
        }
    }

    for (auto &l : vec) {
        std::cout << l << std::endl;
    }

    std::cout << "\nresize" << std::endl;
    vec.resize({2, 3});
    for (auto &l : vec) {
        if (l.empty()) {
            std::cout << "e" << std::endl;
        }
        else {
            std::cout << l << std::endl;
        }
    }

    vec.resize_fill_emplace({2,2}, "asd");
    std::cout << "\n fill resize:\n";
    for (auto &l : vec) {
        std::cout << l << std::endl;
    }
}
