#include <cstddef>
#include <iostream>
template <typename T>
class Linear {
    private:
        char* m_buffer;
        char* m_currPos;
        size_t m_size;

    public:
        Linear(size_t size) : m_size(size * sizeof(T)) {
            m_buffer = new char[m_size];
            m_currPos = m_buffer;
        }
        ~Linear() {
            delete[] m_buffer;
        }

        T* allocate(size_t n) {
            if(m_currPos + (n * sizeof(T))  > m_buffer + m_size) return nullptr;
            T* tmp = reinterpret_cast<T*>(m_currPos) ;
            m_currPos += (n * sizeof(T));
            return tmp;
        }
        void reset() {
            m_currPos = m_buffer;
        }
        
};

int main() {
    Linear<int> allocator(10); 

    int* data = allocator.allocate(5);
    if (data) {
        data[0] = 1;
        data[1] = 2;
        std::cout << << data[0] << "  " << data[1] << std::endl;
    }
}
