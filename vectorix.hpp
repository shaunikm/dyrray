#ifndef VECTORIX_HPP
#define VECTORIX_HPP

#include <memory> // for std::allocator
#include <iterator>
#include <stdexcept>

template <typename T>

class Vectorix {
private:
    T* data;                        // pointer to array's memory block
    std::size_t size;               // number of elements currently in the array
    std::size_t current_capacity;   // allocated memory capacity at current call
    double shrink_sentinel;         // threshold to trigger shrink-to-fit
    std::allocator<T> allocator;    // mem allocator (constructing/destructing)
    
    // reallocate new chunk of data in linear blocks
    void reallocate(std::size_t new_capacity) {
        T* new_data = allocator.allocate(new_capacity);

        try {
            for (std::size_t i = 0; i < size; i++) {
                allocator.construct(&new_data[i], std::move(data[i]))
                allocator.destroy(&data[i]);
            }
        } catch (...) { // ensure deallocation in the case of an exception
            for (std::size_t i = 0; i < size; i++) {
                allocator.destroy(&data[i]);
            }
            allocator.deallocate(new_data, new_capacity);
            throw;
        }

        allocator.deallocate(data, current_capacity);
        data = new_data;
        current_capacity = new_capacity;
    }

    // shrink-to-fit with sentinel value
    void shrink_to_fit() {
        if (size < current_capacity * shrink_sentinel) {
            reallocate(size); // keep only memory needed for current elements
        }
    }

public:
    // iterator support
    using iterator = T*;
    using const_iterator = const T*;

    // constructor
    explicit Vectorix() : data(nullptr), size(0), current_capacity(0), shrink_sentinel(0.5) {}

    // Destructor
    ~Vectorix() {
        allocator.deallocate(data, current_capacity);
    }

}

#endif // VECTORIX_HPP