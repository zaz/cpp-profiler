#include <iostream>

#include "profile.hpp"
profile simple_cpp("simple.cpp");

int search(int tbl[], int n, int key) {
    int result = -1;
    for (int i = 0; i < n; ++i) {
        if (key == tbl[i]) {
            result = i;
        }
    }
    return result;
}

int main() {
    int lst[5] = {2, 4, 6, 8, 10};
    std::cout << search(lst, 5, 6);
    std::cout << std::endl;

    std::cout << "Done";
    std::cout << std::endl;
    return 0;
}
