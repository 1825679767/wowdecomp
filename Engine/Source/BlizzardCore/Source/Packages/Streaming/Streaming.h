#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STREAMING_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_STREAMING_STREAMING_H

#include <cstdint>
#include <map>
#include <string>
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Digest.h"
#include "Engine/Source/BlizzardCore/Source/System/File/Types.h"

namespace Blizzard {
namespace Streaming {

struct FILE_THREAD_CMD {
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ void* unk_0010;
};

struct DownloadSlot {
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ uint32_t unk_0010;
};

struct DigestState {
    /* 0000 */ int32_t unk_0000;
    /* 0004 */ int32_t unk_0004;
};

struct StorageEntry {
    /* 0000 */ uint8_t unk_0000[168];

    ~StorageEntry();
};

class SourceManifest {
    public:
    /* 0000 */ uint8_t unk_0000[80];

    ~SourceManifest();
};

struct ProgressSample {
    /* 0000 */ int64_t unk_0000;
    /* 0008 */ int32_t unk_0008;
    /* 000C */ uint8_t unk_000C[4];
};

typedef void (*PROGRESSCALLBACK)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, int32_t);

class DownloadProgress {
    public:
    /* 0000 */ uint32_t unk_0000[6];
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint8_t unk_001C[4];
    /* 0020 */ ProgressSample unk_0020[51];
    /* 0350 */ uint32_t unk_0350;
    /* 0354 */ PROGRESSCALLBACK unk_0354;
    /* 0358 */ bool unk_0358;

    DownloadProgress();
    ~DownloadProgress();
    int32_t FindNextSlot();
    void FireCallback();
    void ResetTime();
};

typedef std::map<Blizzard::Util::Digest<16>, DigestState> DigestStateMap;
typedef std::map<std::string, std::string> PathMap;
typedef std::map<std::string, StorageEntry> StorageEntryMap;

extern bool s_streamingReady;
extern bool s_isStreamingTrial;

int32_t CheckFileRange(const char* path, int64_t offset, uint32_t count, const char* name, int32_t flags);
uint32_t GetIsLocalAmount(const char* path, int64_t offset, uint32_t size, const char* name);
bool IsStreamedFile(const char* path);
std::string ResolveFilePath(const std::string& path);
bool IsStreamingMode();
void SetZone(int32_t areaId, int32_t subzoneId);
bool ScanDirectoryForManifest(const Blizzard::File::ProcessDirParms& parms);
void MakePartFileName(char* name, uint32_t size);

}
}

class StreamingManager {
    public:
    void Shutdown();
};

bool CheckIfStreamingAndTrial();
void StringToLower(char* str);
StreamingManager* GetStreamingManagerSingleton();
void SetClientInitialized(uint8_t initialized);
void ShutdownStreamingIfTrial();
void StopStreaming();
int32_t StreamingGetRace();
bool StreamingIsRaceDataReady(int32_t raceID);
void StreamingSetAreaOfInterest(int32_t chunkY, int32_t chunkX);
void StreamingSetMapId(int32_t mapId);
void StreamingSetRace(int32_t raceID);
void StreamingSetState(int32_t state);
char* GetComponentVersionData(uint32_t* size);

void SetActiveDataSource(int32_t source);
float StreamingGetLoadFraction(int32_t raceID);

#endif
