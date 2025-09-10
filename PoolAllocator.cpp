#include <iostream>
template <typename T>
class PoolAllocator{
    public:
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using size_type = size_t;
    private:
        void* head;
        void* free_start;
        size_type block_size;
        size_type block_count;
        size_type allocated;

    public:
        PoolAllocator(size_type blc_sz, size_type blc_count) {
            block_size = blc_sz > sizeof(void*) ? blc_sz : sizeof(void*);
            block_count = blc_count;
            head = free_start = ::operator new(block_size * block_count);
            allocated = 0;

            char* curr = reinterpret_cast<char*>(head);
            for (size_type i = 0; i < block_count - 1; ++i) {
                char* next = curr + block_size;
                *reinterpret_cast<void**>(curr) = next;
                curr = next;
            }
            *reinterpret_cast<void**>(curr) = nullptr;
        }

        PoolAllocator(const PoolAllocator& other) : PoolAllocator(other.block_size, other.block_count) {}

        ~PoolAllocator() {
            ::operator delete(head);
        }

        pointer allocate() {
            if (is_full()) throw std::bad_alloc();
            pointer start = static_cast<pointer>(free_start);
            free_start = *reinterpret_cast<void**>(start);
	    ++allocated;
            return start;
        }

        void deallocate(pointer ptr) {
            *reinterpret_cast<void**>(ptr) = free_start;
            free_start = ptr;
            --allocated;
        }
        template <typename...Args>
        pointer construct(pointer ptr, Args&& ... args) {
            return new(ptr) T(std::forward<Args>(args)...);
        }
        void destroy(pointer ptr) {
            ptr-> ~T();
        }
        bool is_full() {
           return allocated == block_count;
        }
        void print_pool() const {
            std::cout << "Pool blocks:\n";
            char* curr = reinterpret_cast<char*>(head);
            for (size_type i = 0; i < block_count; ++i) {
                void* next = *reinterpret_cast<void**>(curr);
                std::cout << "Block[" << i << "] at " << static_cast<void*>(curr)
                          << " -> next = " << next;
                if (curr == free_start)
                    std::cout << "  <-- free_start";
                std::cout << "\n";
                curr += block_size;
            }
            std::cout << "Allocated blocks: " << allocated << "\n\n";
        }
};

int main() {
    PoolAllocator<int> pool(4,5);
    int* ptr = pool.allocate();
    int* ptr1 = pool.allocate();

    pool.deallocate(ptr);
    pool.print_pool();
}

