#include <iostream>
#include <algorithm>

#include "dyrray.hpp"

int main() {
    Dyrray<int> dynamic_array;

    // adding elements
    dynamic_array.push_back(5);
    dynamic_array.push_back(3);
    dynamic_array.push_back(1);

    // accessing elements
    std::cout << "elements in the array: ";
    for (size_t i = 0; i < dynamic_array.get_size(); ++i) {
        std::cout << dynamic_array[i] << " ";
    }
    std::cout << std::endl;

    // accessing out of bounds element (no bounds checking)
    try {
        std::cout << "element at index 2: " << dynamic_array.at(2) << std::endl;
        std::cout << "trying to access out of bounds index..." << std::endl;
        dynamic_array.at(10); // testing if the correct error is thrown
    } catch (const std::out_of_range& e) {
        std::cerr << e.what() << std::endl;
    }

    // pop elements
    dynamic_array.pop_back();
    std::cout << "elements after popping: ";
    for (size_t i = 0; i < dynamic_array.get_size(); ++i) {
        std::cout << dynamic_array[i] << " ";
    }
    std::cout << std::endl;

    // clear array
    dynamic_array.clear();
    std::cout << "size after clearing: " << dynamic_array.get_size() << std::endl;

    // testing STL sorting on array
    std::sort(dynamic_array.begin(), dynamic_array.end());
    for (size_t i = 0; i < dynamic_array.get_size(); ++i) {
        std::cout << dynamic_array[i] << " ";
    }

    return 0;
}
