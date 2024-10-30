#include <iostream>
#include <algorithm>
#include <cassert>

#include "dyrray.hpp"

int main() {
    // 1. test for default construction
    Dyrray<int> array;
    assert(array.get_size() == 0);
    assert(array.get_capacity() == 0);
    std::cout << "Default constructor test passed.\n";

    // 2. test push_back and dynamic resizing
    array.push_back(10);
    assert(array.get_size() == 1);
    assert(array[0] == 10);
    std::cout << "First push_back test passed.\n";
    std::cout << "Current capacity: " << array.get_capacity() << std::endl;

    // Add more elements to test capacity growth
    for (int i = 0; i < 100; ++i) {
        array.push_back(i);
    }
    assert(array.get_size() == 101); // Including the initial push_back
    assert(array[100] == 99);
    std::cout << "Push_back and dynamic resizing test passed.\n";
    std::cout << "Current capacity: " << array.get_capacity() << std::endl;

    // 3. Test access with at()
    try {
        int val = array.at(100); // Should work
        assert(val == 99);
        array.at(101); // Should throw an exception
    } catch (const std::out_of_range& e) {
        std::cout << "Access test with at() passed: " << e.what() << '\n';
    }

    // 4. Test pop_back and shrink-to-fit behavior
    array.pop_back();
    assert(array.get_size() == 100);
    std::cout << "Pop_back test passed.\n";

    // 5. Test clear function
    array.clear();
    assert(array.get_size() == 0);
    std::cout << "Clear function test passed.\n";
    std::cout << "Current capacity: " << array.get_capacity() << std::endl;

    // 6. Test move constructor
    Dyrray<int> array2(std::move(array));
    assert(array.get_size() == 0);   // array should be empty after move
    assert(array2.get_size() == 0);  // array2 should also be empty
    std::cout << "Move constructor test passed.\n";

    // 7. Test copy assignment
    for (int i = 0; i < 10; ++i) {
        array2.push_back(i);
    }
    Dyrray<int> array3;
    array3 = array2;
    assert(array3.get_size() == array2.get_size());
    assert(array3[5] == array2[5]);
    std::cout << "Copy assignment test passed.\n";

    std::cout << "All tests passed successfully.\n";
    return 0;
}
