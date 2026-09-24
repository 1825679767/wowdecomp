#include <cstdint>
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/File/Filestack_Streaming.h"

// VERIFY: 149f4945 manual
// OFFSET: 0x00427BE0
bool System_File::Stacked::stream_read(Blizzard::File::Filesystem* fs, FileParms* parms) {
    Blizzard::File::StreamingInfo* streaming = parms->m_stream->m_streaming;

    if ((streaming && streaming->m_storagehandle.m_mNode) || !fs->m_next) {
        return false;
    }

    return (*reinterpret_cast<FileFunc*>(reinterpret_cast<uintptr_t>(fs->m_next) + parms->m_verb))(fs->m_next, parms);
}
