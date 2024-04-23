#include <iostream>
#include "map.hpp"

int main() {
    Map<int, int, Allocator> m;

    for(size_t i = 0; i < 100; ++i) {
        m.insert(i,i);
    }

    for(auto& a : m) {
        std::cout << a.first << " " << a.second << "\r\n";
    }
}