#if defined(WOWDECOMP_ARCH_32)

#include <cstdint>
#include <cstring>
#include <deque>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"

// STATIC: 0x00B3252C
static HANDLE unk_B3252C;

// STATIC: 0x00B32530
static CRITICAL_SECTION unk_B32530;

// STATIC: 0x00B32548
static HANDLE unk_B32548[2];

// STATIC: 0x00B32550
static HANDLE unk_B32550;

// STATIC: 0x00B32554
static Blizzard::Streaming::DownloadSlot* unk_B32554[2];

// STATIC: 0x00B3255C
static std::deque<Blizzard::Streaming::FILE_THREAD_CMD>* unk_B3255C[7];

// STATIC: 0x00B325A0
static CRITICAL_SECTION unk_B325A0;

// STATIC: 0x00B325B8
static bool unk_B325B8;

// STATIC: 0x00B325C0
static CRITICAL_SECTION unk_B325C0;

// STATIC: 0x00B325D8
static HANDLE unk_B325D8;

// STATIC: 0x00B325DC
static HANDLE unk_B325DC;

// STATIC: 0x00B38170
static Blizzard::Streaming::SourceManifest* unk_B38170;

// STATIC: 0x00B38174
static Blizzard::Streaming::StorageEntryMap* unk_B38174;

// STATIC: 0x00B38178
static Blizzard::File::StreamRecord* unk_B38178;

// STATIC: 0x00B3817C
static bool unk_B3817C;

// STATIC: 0x00B3817E
static bool unk_B3817E;

// STATIC: 0x00B3817F
static bool unk_B3817F;

// STATIC: 0x00B38182
static bool unk_B38182;

// STATIC: 0x00B38184
static int32_t unk_B38184;

// STATIC: 0x00B38294
static Blizzard::Streaming::DigestStateMap* unk_B38294;

// STATIC: 0x00B38298
static Blizzard::Streaming::PathMap* unk_B38298;

// STATIC: 0x00B386D0
static Blizzard::Streaming::DownloadProgress unk_B386D0;

// VERIFY: 63d60986 auto
// OFFSET: 0x004294C0
char* GetComponentVersionData(uint32_t* size) {
    *size = 0;

    HRSRC resource = FindResourceExA(nullptr, "BLIZZARDCOMPONENT", "BLIZZARDCOMPONENT", 0);

    if (!resource) {
        return nullptr;
    }

    HGLOBAL handle = LoadResource(nullptr, resource);

    if (!handle) {
        return nullptr;
    }

    const void* source = LockResource(handle);
    *size = SizeofResource(nullptr, resource);

    char* data = new char[*size + 1];
    data[*size] = '\0';
    memcpy(data, source, *size);

    return data;
}

// VERIFY: 59db56f0 manual
// OFFSET: 0x0044B570
std::string Blizzard::Streaming::ResolveFilePath(const std::string& path) {
    if (!unk_B38298) {
        return std::string();
    }

    if ((*unk_B38298)[path].size()) {
        return (*unk_B38298)[path];
    }

    char absolute[260];

    if (!Blizzard::File::MakeAbsolutePath(path.c_str(), absolute, 260, false)) {
        return std::string();
    }

    char normalized[260];
    char* dst = normalized;
    const char* src = absolute;
    uint32_t left = 260;

    while (true) {
        char c = *src++;
        left--;

        if (!c) {
            *dst = '\0';
            break;
        }

        *dst++ = c == '/' ? '\\' : c;

        if (!left) {
            dst[-1] = '\0';
            break;
        }
    }

    StringToLower(normalized);
    (*unk_B38298)[path].assign(normalized, strlen(normalized));
    return std::string((*unk_B38298)[path]);
}

// VERIFY: 745bac15 manual
// OFFSET: 0x0044EFB0
bool Blizzard::Streaming::IsStreamedFile(const char* path) {
    if (!Blizzard::Streaming::s_streamingReady || !path) {
        return false;
    }

    std::string resolved = Blizzard::Streaming::ResolveFilePath(std::string(path));

    if (!unk_B38174) {
        return false;
    }

    return unk_B38174->find(resolved) != unk_B38174->end();
}

// VERIFY: 5b965645 manual
// OFFSET: 0x004508B0
static void StopThread() {
    if (unk_B3817C && unk_B38174) {
        unk_B3817E = true;
        SetEvent(unk_B325D8);

        while (!unk_B3817F) {
            Sleep(10);
        }

        CloseHandle(unk_B325D8);
        DeleteCriticalSection(&unk_B325C0);
        unk_B325C0.DebugInfo = nullptr;

        if (unk_B38178) {
            Blizzard::File::Close(unk_B38178);
        }
    }

    delete unk_B38174;
    unk_B38174 = nullptr;
}

// VERIFY: a9cb82c4 manual
// OFFSET: 0x00457680
void StopStreaming() {
    if (Blizzard::Streaming::s_streamingReady) {
        unk_B386D0.FireCallback();
        unk_B38182 = true;
        SetEvent(unk_B325DC);
        SetEvent(unk_B3252C);
        unk_B325B8 = false;
        SetEvent(unk_B32550);

        while (unk_B38184 > 0) {
            Sleep(10);
        }

        CloseHandle(unk_B325DC);
        CloseHandle(unk_B3252C);

        for (uint32_t i = 0; i < sizeof(unk_B32548) / sizeof(unk_B32548[0]); i++) {
            CloseHandle(unk_B32548[i]);
            delete unk_B32554[i];
        }

        DeleteCriticalSection(&unk_B32530);
        unk_B32530.DebugInfo = nullptr;
        CloseHandle(unk_B32550);
        DeleteCriticalSection(&unk_B325A0);
        unk_B325A0.DebugInfo = nullptr;

        delete unk_B38294;

        for (uint32_t i = 0; i < sizeof(unk_B3255C) / sizeof(unk_B3255C[0]); i++) {
            delete unk_B3255C[i];
        }

        delete unk_B38298;
        unk_B38298 = nullptr;
        Blizzard::Streaming::s_streamingReady = false;
    }

    StopThread();

    delete unk_B38170;
    unk_B38170 = nullptr;
}

#endif
