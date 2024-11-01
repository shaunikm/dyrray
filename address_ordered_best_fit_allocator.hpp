#ifndef ADDRESS_ORDERED_BEST_FIT_ALLOCATOR_HPP
#define ADDRESS_ORDERED_BEST_FIT_ALLOCATOR_HPP

#include <set>
#include <memory>

/**
 * Memory allocator that uses address-ordered policy and
 * best-fit policy to try to reduce memory fragmentation
 */
template <typename T>
class AddressOrderedBestFitAllocator {
private:
    struct Block {
        T* address;
        std::size_t size;
        bool operator< (const Block& other) const {
            return address < other.address;
        }
    };

    std::allocator<T> allocator;
    std::set<Block> free_blocks;
    std::size_t total_capacity;

    // best-fit policy to find memory block that best fits
    // requested size
    auto find_best_block_fit(std::size_t size) {
        auto best_block_fit = free_blocks.end();

        for (auto it = free_blocks.begin(); it != free_blocks.end(); it++) {
            if (it->size >= size)
                if (best_block_fit == free_blocks.end() || it->size < best_block_fit->size)
                    best_block_fit = it;  
        }

        return best_block_fit;
    }

    // coalesce all free blocks next to each other
    void merge() {
        auto it = free_blocks.begin();

        while (it != free_blocks.end()) {
            auto next = std::next(it);

            // if blocks are next to each other
            if (next != free_blocks.end() && it->address + it->size == next->address) {
                Block merged = {it->address, it->size + next->size};
                it = free_blocks.erase(it);
                next = free_blocks.erase(next);
                free_blocks.insert(merged);
            } else {
                it++;
            }
        }
    }

public:
    AddressOrderedBestFitAllocator() : total_capacity(0) {}

    ~AddressOrderedBestFitAllocator() {
        for (const auto& block : free_blocks) {
            allocator.deallocate(block.address, block.size);
        }
    }

    T* allocate(std::size_t size) {
        auto best_block_fit = find_best_block_fit(size);

        if (best_block_fit != free_blocks.end()) {
            T* addr = best_block_fit->address;
            free_blocks.erase(best_block_fit);

            // free memory not used by requested block
            if (best_block_fit->size > size) {
                free_blocks.insert({addr + size, best_block_fit->size - size});
            }

            return addr;
        } else {
            T* addr = allocator.allocate(size);
            total_capacity += size;

            return addr;
        }
    }

    void deallocate(T* addr, std::size_t size) {
        free_blocks.insert({addr, size});
        merge(); // merge all blocks together again
    }

    std::size_t capacity() const { return total_capacity; }
};

#endif // ADDRESS_ORDERED_BEST_FIT_ALLOCATOR_HPP