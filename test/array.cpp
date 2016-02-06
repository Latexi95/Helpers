#include "array.h"
#include <iostream>

int main() {
    Array<int, 2, 2> arr({
                         {1, 2},
                         {3, 4}
                         });

    for (int i : arr) {
        std::cout << i << std::endl;
    }

    return 0;
}

