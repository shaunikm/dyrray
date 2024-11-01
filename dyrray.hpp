#ifndef DYRRAY_HPP
#define DYRRAY_HPP

#include <memory> // for std::allocator
#include <vector>
#include <iterator>
#include <stdexcept>

#include "address_ordered_best_fit_allocator.hpp"

template <typename T>

class Dyrray {
private:
    T* data;                        // pointer to array's memory block
    std::size_t size;               // number of elements currently in the array
    std::size_t current_capacity;   // allocated memory capacity at current call
    double growth_factor;           // growth factor of memory allocated
    double shrink_sentinel;         // threshold to trigger shrink-to-fit
    AddressOrderedBestFitAllocator<T> allocator;    // custom allocator
    
    // reallocate new chunk of data in linear blocks
    void reallocate(std::size_t new_capacity) {
        T* new_data = allocator.allocate(new_capacity);

        for (std::size_t i = 0; i < size; i++) {
            new(new_data + i) T(std::move(data[i])); // move-construct
            data[i].~T(); // destroy old element
        }

        // deallocate old memory
        allocator.deallocate(data, current_capacity);
        data = new_data;
        current_capacity = new_capacity;
    }

    // shrink-to-fit with custom sentinel value
    void shrink_to_fit() {
        if (size < current_capacity * shrink_sentinel) {
            reallocate(size); // only keep memory needed for current elements
        }
    }

public:
    // iterator support
    using iterator = T*;
    using const_iterator = const T*;

    // constructor
    explicit Dyrray() : data(nullptr), size(0), current_capacity(0), growth_factor(2.0), shrink_sentinel(0.5) {}

    // Destructor
    ~Dyrray() {
        clear();
        allocator.deallocate(data, current_capacity);
    }

    // copy constructor
    Dyrray(const Dyrray& other) : data(allocator.allocate(other.current_capacity)), size(other.size), current_capacity(other.current_capacity), growth_factor(other.growth_factor), shrink_sentinel(other.shrink_sentinel) {
        for (std::size_t i = 0; i < size; i++) {
            allocator.construct(&data[i], other.data[i]);
        }
    }

    // copy assignment
    Dyrray& operator= (const Dyrray& other) {
        if (this != &other) {
            clear();
            allocator.deallocate(data, current_capacity);

            size = other.size;
            current_capacity = other.current_capacity;
            growth_factor = other.growth_factor;
            shrink_sentinel = other.shrink_sentinel;
            
            data = allocator.allocate(current_capacity);
            for (std::size_t i = 0; i < size; i++) {
                new (data + i) T(other.data[i]);
            }
        }
        
        return *this;
    }

    // move constructor
    Dyrray(Dyrray&& other) noexcept : data(other.data), size(other.size), current_capacity(other.current_capacity), growth_factor(other.growth_factor), shrink_sentinel(other.shrink_sentinel) {
        other.data = nullptr;
        other.size = 0;
        other.current_capacity = 0;
    }

    // move assignment
    Dyrray& operator= (Dyrray&& other) noexcept {
        if (this != &other) {
            clear();
            allocator.deallocate(data, current_capacity);

            data = other.data;
            size = other.size;
            current_capacity = other.current_capacity;
            growth_factor = other.growth_factor;
            shrink_sentinel = other.shrink_sentinel;

            other.data = nullptr;
            other.size = 0;
            other.current_capacity = 0;
        }

        return *this;
    }

    // access with bounds checking
    T& at(std::size_t index) {
        if (index >= size) throw std::out_of_range("Index " + std::to_string(index) + " out of range");
        return data[index];
    }

    const T& at(std::size_t index) const {
        if (index >= size) throw std::out_of_range("Index " + std::to_string(index) + " out of range");
        return data[index];
    }

    // add new element
    void push_back(T val) {
        if (size == current_capacity) {
            std::size_t new_capacity = (current_capacity == 0) ? 1 : static_cast<std::size_t>(current_capacity * growth_factor);
            reallocate(new_capacity);
        }

        new(data + size) T(val);
        size++;
    }

    // remove last element
    void pop_back() {
        if (size > 0) {
            size--;
            data[size].~T();
            shrink_to_fit();
        }
    }

    // clear all elements, but retain allocated memory for reuse
    void clear() {
        for (std::size_t i = 0; i < size; i++) {
            data[i].~T();
        }
        size = 0;
        shrink_to_fit();
    }

        // accessor methods for size and current capacity
    std::size_t get_size() const { return size; }
    std::size_t get_capacity() const { return current_capacity; }

    // operator overload for element access
    T& operator[] (std::size_t index) { return data[index]; }
    const T& operator[] (std::size_t index) const { return data[index]; }

    // iterator support
    iterator begin() { return data; }
    const_iterator begin() const { return data;}

    iterator end() { return data + size; }
    const_iterator end() const { return data + size; }
};

#endif // DYRRAY_HPP