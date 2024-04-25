#include <iostream>
#include "allocator.hpp"
#include "double_list.hpp"

int main() {
    DoubleList<int, Allocator> l;

    for(size_t i = 0; i < 100; ++i) {
        l.push_back(i);
    }

    //l.insert(l.end(), 101);

    for(auto& a : l) {
        std::cout << a << "\r\n";
    }
}