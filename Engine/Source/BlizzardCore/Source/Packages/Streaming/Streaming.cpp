#include <cstdint>
#include <cstring>
#include <string>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"

// STATIC: 0x00AB7AC8
static uint8_t s_clientInitialized;

// STATIC: 0x00B38038
static Blizzard::Time::Timestamp s_elapsedSinceStart;

// STATIC: 0x00B38180
bool Blizzard::Streaming::s_streamingReady;

// STATIC: 0x00B38181
bool Blizzard::Streaming::s_isStreamingTrial;

// VERIFY: 19d4f174 manual
// OFFSET: 0x00424280
bool Blizzard::Streaming::ScanDirectoryForManifest(const Blizzard::File::ProcessDirParms& parms) {
    System_SFile2::ScanDirCookie* cookie = static_cast<System_SFile2::ScanDirCookie*>(parms.m_cookie);

    char path[512];
    Blizzard::String::Format(path, sizeof(path), "%s%s%s", cookie->m_root, cookie->m_prefix, parms.m_filename);
    char* relative = &path[cookie->m_rootLength];

    if (parms.m_isDir) {
        if (Blizzard::String::EqualI(parms.m_filename, ".svn")) {
            return true;
        }

        Blizzard::String::ForceTrailingSeparator(path, sizeof(path), '\\');

        if (System_SFile2::s_dataPath[0] && Blizzard::String::Equal(&path[cookie->m_rootLength], System_SFile2::s_dataPath)) {
            return true;
        }

        const char* prefix = cookie->m_prefix;
        cookie->m_prefix = &path[cookie->m_rootLength];
        Blizzard::File::ProcessDirFast(path, cookie, Blizzard::Streaming::ScanDirectoryForManifest, false);
        cookie->m_prefix = prefix;

        return true;
    }

    char* extension = const_cast<char*>(Blizzard::String::FindExtensionWithLeadingDot(path));
    char* fullPath = Blizzard::String::Duplicate(path);

    if (*extension) {
        int32_t type = 0;

        if (Blizzard::String::EqualI(extension, ".bz")) {
            type = 1;
        } else if (Blizzard::String::EqualI(extension, ".MPQ")) {
            type = 2;
        }

        if (type) {
            *extension = 0;

            if (System_SFile2::s_fileHash.find(std::string(relative)) == System_SFile2::s_fileHash.end()) {
                System_SFile2::FileHashEntry entry;
                entry.m_path = fullPath;
                entry.m_type = type;
                System_SFile2::s_fileHash.insert(System_SFile2::FileHash::value_type(std::string(relative), entry));

                Blizzard::Memory::Free(fullPath);

                return true;
            }
        }
    }

    if (System_SFile2::s_fileHash.find(std::string(relative)) == System_SFile2::s_fileHash.end()) {
        System_SFile2::FileHashEntry entry;
        entry.m_path = fullPath;
        entry.m_type = 0;
        System_SFile2::s_fileHash.insert(System_SFile2::FileHash::value_type(std::string(relative), entry));
    }

    Blizzard::Memory::Free(fullPath);

    return true;
}

// VERIFY: 43a5806e auto
// OFFSET: 0x00428000
bool Blizzard::Streaming::IsStreamingMode() {
    return Blizzard::Streaming::s_streamingReady;
}

// VERIFY: f236703c manual
// OFFSET: 0x00428010
bool CheckIfStreamingAndTrial() {
    return Blizzard::Streaming::s_streamingReady && Blizzard::Streaming::s_isStreamingTrial;
}

// VERIFY: 657b98b1 auto
// OFFSET: 0x0044F0D0
void Blizzard::Streaming::MakePartFileName(char* name, uint32_t size) {
    if (!IsStreamedFile(name)) {
        return;
    }

    char part[260];
    Blizzard::Storage::BuildVariantFileName(part, sizeof(part), name, "part");
    Blizzard::String::Copy(name, part, size);
}

// VERIFY: 76edbcda auto
// OFFSET: 0x00437A00
void Blizzard::Time::GetElapsedSinceStart(bool start) {
    // STATIC: 0x00B38A48
    static Blizzard::Time::Timestamp s_startTime = Blizzard::Time::GetTimestamp();

    if (!start) {
        s_elapsedSinceStart = Blizzard::Time::GetTimestamp() - s_startTime;
    }
}

// INLINED
Blizzard::Streaming::DownloadProgress::DownloadProgress() {
    this->ResetTime();
}

// INLINED
Blizzard::Streaming::DownloadProgress::~DownloadProgress() {
    this->unk_0018 = 0;
    this->unk_0354 = nullptr;
    memset(this->unk_0020, 0, sizeof(this->unk_0020));
}

// VERIFY: 1ed002c0 manual
// OFFSET: 0x00438300
void Blizzard::Streaming::DownloadProgress::ResetTime() {
    this->unk_0018 = static_cast<uint32_t>(Blizzard::Time::Milliseconds());
}

// VERIFY: d9c84ab5 auto
// OFFSET: 0x00420930
void ShutdownStreamingIfTrial() {
    if (CheckIfStreamingAndTrial()) {
        GetStreamingManagerSingleton()->Shutdown();
    }
}

// VERIFY: 5fde920f auto
// OFFSET: 0x0041E4E0
void SetClientInitialized(uint8_t initialized) {
    s_clientInitialized = initialized;
}
