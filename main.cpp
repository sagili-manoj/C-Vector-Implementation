#include "customVector.hpp"
#include <iostream>
#include <vector>
int main() {
    // Example 1: Basic usage
    Vector<int> v;
    v.push_back(1);
    v.emplace_back(2);
    v.push_back(3);
    std::cout << "Size: " << v.size() << ", Capacity: " << v.capacity() << "\n";
    for (auto it = v.begin(); it != v.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";

    // Example 2: From iterator range (random access)
    std::vector<int> src = {10, 20, 30, 40};
    Vector<int> v2(src.begin(), src.end());
    std::cout << "Vector from iterator range: ";
    for (const auto& x : v2) {
        std::cout << x << " ";
    }
    std::cout << "\nSize: " << v2.size() << ", Capacity: " << v2.capacity() << "\n";

    // Example 3: Resize and shrink_to_fit
    v.resize(5, 42);
    std::cout << "After resize(5, 42): ";
    for (const auto& x : v) {
        std::cout << x << " ";
    }
    std::cout << "\n";
    v.shrink_to_fit();
    std::cout << "After shrink_to_fit: Size: " << v.size() << ", Capacity: " << v.capacity() << "\n";

    return 0;
}
