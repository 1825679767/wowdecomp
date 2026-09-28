#ifndef ENGINE_SOURCE_BASE_CDATAALLOCATOR_H
#define ENGINE_SOURCE_BASE_CDATAALLOCATOR_H

#include <cstdint>
#include <new>
#include <typeinfo>
#include "Engine/Source/Storm/H/storm.h"

class CDataAllocator {
    public:
    struct Block {
        /* 0000 */ Block* m_next;
    };

    struct Data {
        /* 0000 */ Data* m_next;
    };

    /* 0000 */ uint32_t m_bytesPerData;
    /* 0004 */ uint32_t m_dataPerBlock;
    /* 0008 */ uint32_t m_dataUsed;
    /* 000C */ Block* m_blockList;
    /* 0010 */ Data* m_dataList;

    CDataAllocator(uint32_t bytesPerData, uint32_t dataPerBlock);
    ~CDataAllocator();
    void Clear(const char* fileName, int32_t lineNumber, int32_t reportLeaks);
    void* GetData(int32_t zero, const char* fileName, int32_t lineNumber);
    void PutData(void* data, const char* fileName, int32_t lineNumber);

    template <class T>
    T* New(int32_t zero);
};

// VERIFY: 6cf1bf27 manual
// OFFSET: [0x007D4920]
template <class T>
T* CDataAllocator::New(int32_t zero) {
    void* data = this->GetData(zero, typeid(T).raw_name(), SLOG_OBJECT);
    return data ? new (data) T : nullptr;
}

#endif
