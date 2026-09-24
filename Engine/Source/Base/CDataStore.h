#ifndef ENGINE_SOURCE_BASE_CDATASTORE_H
#define ENGINE_SOURCE_BASE_CDATASTORE_H

#include <cstdint>

class CDataStore {
    public:
    /* 0004 */ uint8_t* m_data;
    /* 0008 */ uint32_t m_base;
    /* 000C */ uint32_t m_alloc;
    /* 0010 */ uint32_t m_size;
    /* 0014 */ uint32_t m_read;

    virtual void InternalInitialize(uint8_t*& data, uint32_t& base, uint32_t& alloc);
    virtual void InternalDestroy(uint8_t*& data, uint32_t& base, uint32_t& alloc);
    virtual int32_t InternalFetchRead(uint32_t pos, uint32_t bytes, uint8_t*& data, uint32_t& base, uint32_t& alloc);
    virtual int32_t InternalFetchWrite(uint32_t pos, uint32_t bytes, uint8_t*& data, uint32_t& base, uint32_t& alloc, const char* fileName, int32_t lineNumber);
    virtual ~CDataStore();
    virtual int32_t IsRead();
    virtual void Reset();
    virtual void Finalize();
    virtual void GetBufferParams(const void** data, uint32_t* size, uint32_t* alloc);
    virtual void DetachBuffer(void** data, uint32_t* size, uint32_t* alloc);
    virtual uint32_t GetHeaderSpace(int32_t bytes);

    CDataStore();
    int32_t FetchRead(uint32_t pos, uint32_t bytes);
    int32_t FetchWrite(uint32_t pos, uint32_t bytes, const char* fileName, int32_t lineNumber);
    CDataStore& Get(uint8_t& val);
    CDataStore& Get(uint16_t& val);
    CDataStore& Get(uint32_t& val);
    CDataStore& Get(uint64_t& val);
    CDataStore& Get(float& val);
    CDataStore& GetArray(uint8_t* pval, uint32_t count);
    CDataStore& GetArray(uint32_t* pval, uint32_t count);
    CDataStore& GetData(void* data, uint32_t bytes);
    CDataStore& GetDataInSitu(void*& data, uint32_t bytes);
    CDataStore& GetString(char* string, uint32_t maxChars);
    CDataStore& Put(uint8_t val);
    CDataStore& Put(uint16_t val);
    CDataStore& Put(uint32_t val);
    CDataStore& Put(uint64_t val);
    CDataStore& Put(float val);
    CDataStore& PutArray(const uint8_t* pval, uint32_t count);
    CDataStore& PutData(const void* data, uint32_t bytes);
    CDataStore& PutString(const char* string);
    CDataStore& Set(uint32_t pos, uint8_t val);
    CDataStore& Set(uint32_t pos, uint16_t val);
    CDataStore& Set(uint32_t pos, uint32_t val);
};

#endif
