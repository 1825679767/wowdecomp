#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SComp.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/TempBuffer.h"
#include "Engine/Source/Storm/Source/SComp.h"
#include "ThirdParty/zlib/contrib/blast/blast.h"
#include "ThirdParty/zlib/zlib.h"

// STATIC: 0x00B38324
Blizzard::Thread::LocalObject<System_SComp::ZlibAllocBuffer> System_SComp::s_zlibAllocBuffer;

// VERIFY: 8d0660e7 manual
// OFFSET: 0x0042BE30
int32_t System_SComp::RLEZeroDecompress(void* dest, uint32_t* destSize, const void* source, uint32_t sourceSize) {
    uint8_t* output = static_cast<uint8_t*>(dest);
    const uint8_t* input = static_cast<const uint8_t*>(source);
    const uint8_t* end = input + sourceSize;

    if (sourceSize < 5) {
        return 0;
    }

    uint32_t remaining = (input[0] << 24) | (input[1] << 16) | (input[2] << 8) | input[3];
    *destSize = remaining;
    input += 4;

    while (input < end) {
        int8_t control = static_cast<int8_t>(*input++);
        uint32_t count;

        if (control >= 0) {
            count = (control & 0x7F) + 3;
            count = count >= remaining ? remaining : count;
            memset(output, 0, count);
        } else {
            count = (control & 0x7F) + 1;
            count = count >= remaining ? remaining : count;
            memcpy(output, input, count);
            input += count;
        }

        output += count;
        remaining -= count;
    }

    return 1;
}

// VERIFY: 1e9af3a0 manual
// OFFSET: 0x00431300
voidpf System_SComp::ZlibAlloc(voidpf opaque, uInt items, uInt size) {
    ZlibAllocBuffer* buffer = static_cast<ZlibAllocBuffer*>(opaque);
    uint32_t bytes = items * size;

    if (bytes & 3) {
        bytes += 4 - (bytes & 3);
    }

    uint8_t* result = buffer->m_next;

    if (bytes >= static_cast<uint32_t>(buffer->m_buffer + sizeof(buffer->m_buffer) - result)) {
        return Blizzard::Memory::Allocate(bytes);
    }

    buffer->m_next = result + bytes;
    return result;
}

// VERIFY: 7b0034c4 manual
// OFFSET: 0x00431350
void System_SComp::ZlibFree(voidpf opaque, voidpf address) {
    ZlibAllocBuffer* buffer = static_cast<ZlibAllocBuffer*>(opaque);

    if (address < buffer->m_buffer || address >= buffer->m_buffer + sizeof(buffer->m_buffer)) {
        Blizzard::Memory::Free(address);
    }
}

// VERIFY: 6e3399c3 manual
// OFFSET: 0x00431380
bool __stdcall System_SComp::DecompressBlock(void* dst, uint32_t* dstSize, const void* src, uint32_t srcSize, Decompressor* decompressors) {
    uint32_t capacity = *dstSize;

    if (!decompressors || !decompressors[0]) {
        return false;
    }

    uint32_t count = 1;

    while (count < 2 && decompressors[count]) {
        count++;
    }

    Blizzard::Memory::TempBuffer<uint8_t, 0x4000> scratch(0);
    uint8_t* output = static_cast<uint8_t*>(dst);
    const uint8_t* input = static_cast<const uint8_t*>(src);
    uint32_t inputSize = srcSize;

    if (count >= 2 || (output + capacity > input && input + srcSize > output)) {
        scratch.Resize(capacity);
    }

    for (uint32_t i = 0; i < count; i++) {
        uint8_t* target = ((count - i) & 1) ? static_cast<uint8_t*>(dst) : scratch.m_data;

        if (target + capacity > input && input + inputSize > target) {
            if (target + capacity <= scratch.m_data || scratch.m_data + inputSize <= target) {
                target = scratch.m_data;
            } else {
                target = static_cast<uint8_t*>(dst);
            }
        }

        uint32_t size = capacity;

        if (!decompressors[i](target, &size, input, inputSize)) {
            return false;
        }

        inputSize = size;
        input = target;
    }

    if (input != dst) {
        memcpy(dst, input, inputSize);
    }

    *dstSize = inputSize;
    return true;
}

// VERIFY: 6959d98d auto
// OFFSET: 0x00453B20
int System_SComp::zlib_decompress(uint8_t* dest, uint32_t* destLen, const uint8_t* source, uint32_t sourceLen) {
    z_stream stream;
    stream.next_in = const_cast<Bytef*>(source);
    stream.avail_in = sourceLen;
    stream.next_out = dest;
    stream.avail_out = *destLen;

    ZlibAllocBuffer* buffer = &static_cast<ZlibAllocBuffer&>(s_zlibAllocBuffer);
    buffer->m_next = buffer->m_buffer;
    stream.zalloc = ZlibAlloc;
    stream.zfree = ZlibFree;
    stream.opaque = buffer;

    int result = inflateInit_(&stream, "1.2.3", sizeof(stream));

    if (result != Z_OK) {
        return result;
    }

    result = inflate(&stream, Z_FINISH);

    if (result != Z_STREAM_END) {
        inflateEnd(&stream);
        return result ? result : Z_BUF_ERROR;
    }

    *destLen = stream.total_out;
    return inflateEnd(&stream);
}

// VERIFY: 4525b2cc auto
// OFFSET: 0x00453BB0
int32_t System_SComp::ZlibDecompress(void* dest, uint32_t* destSize, const void* source, uint32_t sourceSize) {
    uint32_t size = *destSize;

    if (zlib_decompress(static_cast<uint8_t*>(dest), &size, static_cast<const uint8_t*>(source), sourceSize)) {
        return 0;
    }

    *destSize = size;
    return 1;
}

// VERIFY: f56ff40c manual
// OFFSET: 0x00453BF0
void* System_SComp::alloc_wrap(uint32_t size) {
    ZlibAllocBuffer* buffer = &static_cast<ZlibAllocBuffer&>(s_zlibAllocBuffer);
    uint32_t bytes = size;

    if (size & 3) {
        bytes = size - (size & 3) + 4;
    }

    uint8_t* result = buffer->m_next;

    if (bytes >= static_cast<uint32_t>(buffer->m_buffer + sizeof(buffer->m_buffer) - result)) {
        return Blizzard::Memory::Allocate(bytes);
    }

    buffer->m_next = result + bytes;
    return result;
}

// VERIFY: 26a6ae96 manual
// OFFSET: 0x00453C40
void System_SComp::free_wrap(void* address) {
    ZlibAllocBuffer* buffer = &static_cast<ZlibAllocBuffer&>(s_zlibAllocBuffer);

    if (address < buffer->m_buffer || address >= buffer->m_buffer + sizeof(buffer->m_buffer)) {
        Blizzard::Memory::Free(address);
    }
}

// VERIFY: c8e5e01d auto
// OFFSET: 0x00453C70
int32_t System_SComp::LzmaDecompress(void* dest, uint32_t* destSize, const void* source, uint32_t sourceSize) {
    ZlibAllocBuffer* buffer = &static_cast<ZlibAllocBuffer&>(s_zlibAllocBuffer);
    buffer->m_next = buffer->m_buffer;
    return LzmaMemDecode(static_cast<const uint8_t*>(source), sourceSize, static_cast<uint8_t*>(dest), *destSize, destSize, alloc_wrap, free_wrap);
}

// VERIFY: 89255c8c auto
// OFFSET: 0x004268F0
unsigned int System_SComp::PkwareBufferRead(void* param, unsigned char** buffer) {
    PkwareBuffer* pkware = static_cast<PkwareBuffer*>(param);
    *buffer = const_cast<unsigned char*>(pkware->m_input);
    return pkware->m_inputSize;
}

// VERIFY: 68a4cabf auto
// OFFSET: 0x00426910
int System_SComp::PkwareBufferWrite(void* param, unsigned char* data, unsigned int size) {
    PkwareBuffer* pkware = static_cast<PkwareBuffer*>(param);
    memcpy(pkware->m_output + pkware->m_outputPos, data, size);
    pkware->m_outputPos += size;
    return 0;
}

// VERIFY: c24ff103 auto
// OFFSET: 0x00426940
int32_t System_SComp::PkwareDecompress(void* dest, uint32_t* destSize, const void* source, uint32_t sourceSize) {
    PkwareBuffer buffer;
    buffer.m_output = static_cast<uint8_t*>(dest);
    buffer.m_outputPos = 0;
    buffer.m_outputSize = *destSize;
    buffer.m_input = static_cast<const uint8_t*>(source);
    buffer.m_inputSize = sourceSize;

    if (blast(PkwareBufferRead, &buffer, PkwareBufferWrite, &buffer)) {
        return 0;
    }

    *destSize = buffer.m_outputPos;
    return 1;
}

// VERIFY: d59557d7 manual
// OFFSET: 0x004585F0
bool System_SComp::Decompress(void* dst, uint32_t* dstSize, const void* src, uint32_t srcSize) {
    if (*dstSize < srcSize || !srcSize) {
        return false;
    }

    if (srcSize == *dstSize) {
        if (dst != src) {
            memcpy(dst, src, srcSize);
        }

        return true;
    }

    int32_t type = *static_cast<const char*>(src);
    Decompressor decompressors[2] = { nullptr, nullptr };

    switch (type) {
        case 0x02:
            decompressors[0] = ZlibDecompress;
            break;
        case 0x08:
            decompressors[0] = PkwareDecompress;
            break;
        case 0x10:
            decompressors[0] = ::Bzip2Decompress;
            break;
        case 0x12:
            decompressors[0] = LzmaDecompress;
            break;
        case 0x20:
            decompressors[0] = RLEZeroDecompress;
            break;
        case 0x22:
            decompressors[0] = ZlibDecompress;
            decompressors[1] = RLEZeroDecompress;
            break;
        case 0x30:
            decompressors[0] = ::Bzip2Decompress;
            decompressors[1] = RLEZeroDecompress;
            break;
        default:
            Blizzard::File::AddToLastErrorStack(type, "System_SComp::Decompress - Unrecognized Compression Type", 4);
            return false;
    }

    return DecompressBlock(dst, dstSize, static_cast<const uint8_t*>(src) + 1, srcSize - 1, decompressors);
}
