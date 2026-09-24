#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_MEMORY_ALLOCSTL_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_MEMORY_ALLOCSTL_H

#include <cstddef>

namespace Blizzard {
namespace Memory {

template <class T>
class AllocSTL_raw {
    public:
    typedef T value_type;

    AllocSTL_raw();
    template <class U>
    AllocSTL_raw(const AllocSTL_raw<U>& other);

    T* allocate(size_t count);
    void deallocate(T* ptr, size_t count);
};

template <class T, class U>
bool operator==(const AllocSTL_raw<T>& a, const AllocSTL_raw<U>& b);
template <class T, class U>
bool operator!=(const AllocSTL_raw<T>& a, const AllocSTL_raw<U>& b);

}
}

template <class T>
Blizzard::Memory::AllocSTL_raw<T>::AllocSTL_raw() {
}

template <class T>
template <class U>
Blizzard::Memory::AllocSTL_raw<T>::AllocSTL_raw(const AllocSTL_raw<U>& other) {
}

template <class T>
T* Blizzard::Memory::AllocSTL_raw<T>::allocate(size_t count) {
    return static_cast<T*>(::operator new(count * sizeof(T)));
}

template <class T>
void Blizzard::Memory::AllocSTL_raw<T>::deallocate(T* ptr, size_t count) {
    ::operator delete(ptr);
}

template <class T, class U>
bool Blizzard::Memory::operator==(const AllocSTL_raw<T>& a, const AllocSTL_raw<U>& b) {
    return true;
}

template <class T, class U>
bool Blizzard::Memory::operator!=(const AllocSTL_raw<T>& a, const AllocSTL_raw<U>& b) {
    return false;
}

#endif
