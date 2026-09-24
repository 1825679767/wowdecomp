#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SCOMP_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SCOMP_H

#include <cstdint>
#include "Engine/Source/BlizzardCore/Source/System/Thread/ThreadLocalStorage.h"

extern "C" int LzmaMemDecode(const uint8_t* source, uint32_t sourceSize, uint8_t* dest, uint32_t destSize, uint32_t* destProcessed, void* (*alloc)(uint32_t size), void (*free)(void* address));

namespace System_SComp {

typedef int32_t (*Decompressor)(void* dest, uint32_t* destSize, const void* source, uint32_t sourceSize);

struct PkwareBuffer {
    /* 0000 */ uint8_t* m_output;
    /* 0004 */ uint32_t m_outputPos;
    /* 0008 */ uint32_t m_outputSize;
    /* 000C */ const uint8_t* m_input;
    /* 0010 */ uint32_t m_inputSize;
};

struct ZlibAllocBuffer {
    /* 00000 */ uint8_t m_buffer[300000];
    /* 493E0 */ uint8_t* m_next;
};

extern Blizzard::Thread::LocalObject<ZlibAllocBuffer> s_zlibAllocBuffer;

void* alloc_wrap(uint32_t size);
bool Decompress(void* dst, uint32_t* dstSize, const void* src, uint32_t srcSize);
bool __stdcall DecompressBlock(void* dst, uint32_t* dstSize, const void* src, uint32_t srcSize, Decompressor* decompressors);
void free_wrap(void* address);
int32_t LzmaDecompress(void* dest, uint32_t* destSize, const void* source, uint32_t sourceSize);
unsigned int PkwareBufferRead(void* param, unsigned char** buffer);
int PkwareBufferWrite(void* param, unsigned char* data, unsigned int size);
int32_t PkwareDecompress(void* dest, uint32_t* destSize, const void* source, uint32_t sourceSize);
int32_t RLEZeroDecompress(void* dest, uint32_t* destSize, const void* source, uint32_t sourceSize);
void* ZlibAlloc(void* opaque, unsigned int items, unsigned int size);
int32_t ZlibDecompress(void* dest, uint32_t* destSize, const void* source, uint32_t sourceSize);
void ZlibFree(void* opaque, void* address);
int zlib_decompress(uint8_t* dest, uint32_t* destLen, const uint8_t* source, uint32_t sourceLen);

}

#endif
