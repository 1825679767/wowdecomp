#include <cstdint>
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"

// STATIC: 0x00B38038
static Blizzard::Time::Timestamp s_elapsedSinceStart;

// STATIC: 0x00B38180
bool Blizzard::Streaming::s_streamingReady;

// STATIC: 0x00B38181
bool Blizzard::Streaming::s_isStreamingTrial;

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
