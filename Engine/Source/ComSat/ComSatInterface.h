#ifndef ENGINE_SOURCE_COMSAT_COMSATINTERFACE_H
#define ENGINE_SOURCE_COMSAT_COMSATINTERFACE_H

#include <cstddef>
#include <cstdint>

namespace ComSat {

class Allocator {
    public:
    virtual ~Allocator();
    virtual void* Alloc(uint32_t size) = 0;
    virtual void Free(void* ptr) = 0;
};

class Interface {
    public:
    static void Destroy(Interface* comsat, Allocator& allocator);

    virtual ~Interface();
};

namespace Network {

class Manager {
    public:
    ~Manager();
};

}

}

#endif
