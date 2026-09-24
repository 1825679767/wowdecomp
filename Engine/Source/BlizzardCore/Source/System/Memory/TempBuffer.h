#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_MEMORY_TEMPBUFFER_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_MEMORY_TEMPBUFFER_H

#include <cstdint>

namespace Blizzard {
namespace Memory {

template <class T, uint32_t N>
class TempBuffer {
    public:
    /* 0000 */ uint32_t m_count;
    /* 0004 */ T* m_data;
    /* 0008 */ T m_buffer[N];

    TempBuffer(uint32_t count);
    ~TempBuffer();
};

}
}

// INLINED
template <class T, uint32_t N>
Blizzard::Memory::TempBuffer<T, N>::TempBuffer(uint32_t count) {
    this->m_count = count;
    this->m_data = count > N ? new T[count] : this->m_buffer;
}

// VERIFY: 41ffe076 manual
// OFFSET: 0x0042ED70
template <class T, uint32_t N>
Blizzard::Memory::TempBuffer<T, N>::~TempBuffer() {
    if (this->m_data != this->m_buffer) {
        delete[] this->m_data;
    }
}

#endif
