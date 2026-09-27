#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Client.h"
#include "_Triage/AreaList.h"
#include "_Triage/StartupReport.h"
#include "Engine/Source/Base/Base.h"
#include "Engine/Source/Base/CmdLine.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Os/Gui.h"
#include "Engine/Source/Os/OsIME.h"
#include "Engine/Source/Os/Path.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Os/Time.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/Profile.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/MD5.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Source/ConsoleDevice/ConsoleDevice.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/AddOns.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WowServices/PatchFiles.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

namespace {

class ForEveryRunOnceWTF {
    public:
    /* 0000 */ TSGrowableArray<char*> m_files;

    static int32_t AppendFileName(OS_FILE_DATA& data, void* param);
    static int FileNameCompare(const void* a, const void* b);

    void Execute(int32_t (*callback)(const char*));
};

}

// STATIC: 0x009E1BE0
static const _ARGLIST s_wowArgList[] = {
    { 0, CMD_RES_800x600, "800x600", nullptr },
    { 0, CMD_RES_1024x768, "1024x768", nullptr },
    { 0, CMD_RES_1280x960, "1280x960", nullptr },
    { 0, CMD_RES_1280x1024, "1280x1024", nullptr },
    { 0, CMD_RES_1600x1200, "1600x1200", nullptr },
    { 0, CMD_16_BIT, "16bit", nullptr },
    { 0, CMD_UP_TO_DATE, "uptodate", nullptr },
    { 0, CMD_NO_SOUND, "nosound", nullptr },
    { 0, CMD_SOUND_CHAOS, "soundchaos", nullptr },
    { 0, CMD_NO_FIX_LAG, "nofixlag", nullptr },
    { 0, CMD_DEPTH_16, "d16", nullptr },
    { 0, CMD_DEPTH_24, "d24", nullptr },
    { 0, CMD_DEPTH_32, "d32", nullptr },
    { 0, CMD_WINDOWED, "windowed", nullptr },
    { 0, CMD_HW_DETECT, "hwdetect", nullptr },
    { 0, CMD_CONSOLE, "console", nullptr },
    { 0x20000, CMD_GX_OVERRIDE, "gxoverride", nullptr },
};

// STATIC: 0x00AB6164
static uint32_t s_unk_AB6164 = 0x40;

// STATIC: 0x00AB6168
static ARCHIVEINFO s_archives[28] = {
    { "alternate.MPQ", 3, 3, false },
    { "interface.MPQ", 1, 1, false },
    { "misc.MPQ", 1, 1, false },
    { "model.MPQ", 1, 1, false },
    { "texture.MPQ", 1, 1, false },
    { "terrain.MPQ", 1, 1, false },
    { "wmo.MPQ", 1, 1, false },
    { "sound.MPQ", 1, 1, false },
    { "fonts.MPQ", 1, 1, false },
    { "dbc.MPQ", 1, 1, false },
    { "speech.MPQ", 1, 1, false },
    { "expansionloc.MPQ", 2, 1, false },
    { "lichkingloc.MPQ", 2, 1, false },
    { "expansionspeech.MPQ", 2, 1, false },
    { "lichkingspeech.MPQ", 2, 1, false },
    { "expansion.MPQ", 2, 3, false },
    { "lichking.MPQ", 2, 3, false },
    { "common.MPQ", 1, 2, false },
    { "common-2.MPQ", 2, 2, false },
    { "****\\locale-****.MPQ", 1, 2, false },
    { "****\\speech-****.MPQ", 1, 2, false },
    { "****\\expansion-locale-****.MPQ", 2, 2, false },
    { "****\\lichking-locale-****.MPQ", 2, 2, false },
    { "****\\expansion-speech-****.MPQ", 2, 2, false },
    { "****\\lichking-speech-****.MPQ", 2, 2, false },
    { "development.MPQ", 2, 1, false },
    { "streaming.MPQ", 4, 1, false },
    { "streamingloc.MPQ", 4, 1, false },
};
// STATIC: 0x009E0DA4
static const uint8_t s_signatureExponent[4] = { 0x01, 0x00, 0x01, 0x00 };

// STATIC: 0x00AB6028
static const uint8_t s_signatureModulus[256] = {
    0x4F, 0x6F, 0xD4, 0x65, 0x56, 0x85, 0x88, 0xD9, 0xAA, 0xE6, 0xF7, 0x05, 0x72, 0xBB, 0x2A, 0x20,
    0xE0, 0x24, 0x38, 0x2B, 0x77, 0xB3, 0x3D, 0xC2, 0xD3, 0x2D, 0x52, 0x5C, 0xB7, 0xA3, 0x0F, 0xE6,
    0x0F, 0x80, 0x93, 0x8D, 0x5C, 0xC4, 0x81, 0x5E, 0x9D, 0x58, 0x8E, 0xCA, 0xA6, 0x6F, 0x27, 0x36,
    0x52, 0x58, 0x0D, 0xCE, 0x65, 0xE2, 0xB5, 0x7A, 0xD2, 0x39, 0x74, 0x8A, 0xAC, 0xAA, 0xC1, 0x0E,
    0xE5, 0x67, 0x2B, 0x79, 0xC1, 0xFE, 0x08, 0x96, 0x54, 0x18, 0x2A, 0x34, 0x31, 0xA4, 0x06, 0x20,
    0x4B, 0x12, 0xD2, 0x47, 0x63, 0x53, 0x36, 0x38, 0x19, 0x88, 0x8E, 0x50, 0x2F, 0x2F, 0xD1, 0x68,
    0xC8, 0x5C, 0x9D, 0x3B, 0xDE, 0xFE, 0x56, 0x21, 0xD1, 0x59, 0x4D, 0x1C, 0x32, 0x1B, 0x70, 0x4B,
    0x40, 0xE2, 0x5E, 0x0C, 0x0E, 0x4B, 0x84, 0xD5, 0xD7, 0x52, 0x00, 0x50, 0x65, 0x81, 0xCE, 0x17,
    0x53, 0x25, 0x49, 0x9E, 0x40, 0xE9, 0x04, 0x94, 0x2C, 0xA7, 0x93, 0x5C, 0x5D, 0x54, 0xC8, 0xF9,
    0x6E, 0xD0, 0x69, 0xFC, 0xE5, 0x8F, 0xC0, 0xB6, 0x76, 0xF2, 0xEB, 0x34, 0x67, 0x9F, 0xF8, 0xD0,
    0x34, 0xB9, 0xEA, 0x9E, 0x80, 0xD0, 0x29, 0x8D, 0xB4, 0xB2, 0xEB, 0x3E, 0xF4, 0x4F, 0xD0, 0x84,
    0x43, 0x15, 0xF4, 0x19, 0xE3, 0xC6, 0x52, 0xA2, 0x18, 0x40, 0x08, 0x07, 0x21, 0xF4, 0x20, 0xEC,
    0xE6, 0x5A, 0x9C, 0x2B, 0x08, 0xF6, 0x17, 0x97, 0x72, 0xA6, 0xD7, 0xAB, 0x64, 0x0B, 0x04, 0x95,
    0x93, 0x0D, 0xCC, 0x8F, 0x02, 0x30, 0x91, 0xA2, 0xEE, 0x30, 0xE6, 0x5F, 0x2F, 0xD4, 0x13, 0xB5,
    0x5D, 0xCA, 0x5D, 0xCF, 0x7D, 0xC3, 0x6F, 0x6B, 0x9D, 0xBC, 0x6D, 0x86, 0x4C, 0x91, 0xC5, 0x51,
    0xE8, 0xFD, 0xA6, 0x33, 0x10, 0xD1, 0x12, 0x8F, 0x05, 0x80, 0xDA, 0x42, 0x57, 0x0C, 0xEB, 0xC0,
};
// STATIC: 0x00AB6158
static const char* s_dataDirectories[3] = { "Data\\", "..\\Data\\", "Data\\" };

// STATIC: 0x00AB6350
WowClientDB<Startup_StringsRec> g_startup_StringsDB;

// STATIC: 0x00AD2FE0
static const char* s_iniLocaleNames[8] = { "enUS", "koKR", "frFR", "deDE", "zhCN", "zhTW", "esES", "esMX" };

// STATIC: 0x00B2F99C
static int32_t s_region;

// STATIC: 0x00B2F9E1
uint8_t s_expansionLevel;

// STATIC: 0x00B2FA14
static uint32_t s_numPatchArchives;

// STATIC: 0x00B2FA18
static uint32_t s_firstBaseArchive;

// STATIC: 0x00B2FA38
static TSGrowableArray<SArchive*> s_archiveHandles;

// STATIC: 0x00B2FE48
static char s_iniLocale[5];
// STATIC: 0x00B2F050
static uint32_t s_streamingDiag;

// STATIC: 0x00B2F059
static char s_streamingError[2051];

// STATIC: 0x00B2F994
HEVENTCONTEXT g_clientEventContext;

// STATIC: 0x00B2F9D8
static uint32_t s_timeTestError;

// STATIC: 0x00B2FA10
static uint32_t s_unk_B2FA10;

// STATIC: 0x00B2FA48
static char s_installLocation[1024];

// STATIC: 0x00C5DE88
static char s_localeCode[5];

// STATIC: 0x00C5DE90
bool g_hasIsoLocale[12];

// STATIC: 0x00C5DE9C
int32_t s_locale;

// VERIFY: 3b41234e auto
// OFFSET: 0x004022E0
static bool ProcessAffinityMaskCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t maxMask = 0xFFFFFFFF >> (32 - OsGetNumberOfProcessors());
    uint32_t mask = atol(value);

    if (mask > maxMask) {
        ConsolePrintf("Specified mask %08x is greater than the maximum allowable value of %08x", mask, maxMask);
        return false;
    }

    return true;
}

// OFFSET: 0x004029B0
static bool IsPTRBuild() {
    bool isPTR = false;
    uint32_t size = 0;
    char* data = GetComponentVersionData(&size);

    if (data) {
        XMLTree* tree = XMLTree_Load(data, size);

        if (tree) {
            XMLNode* root = XMLTree_GetRoot(tree)->m_child;

            if (root) {
                XMLNode* product = root->GetChildByName("product");

                if (product && !SStrCmp(product->m_body, "WoWT", STORM_MAX_STR)) {
                    isPTR = true;
                }
            }

            XMLTree_Free(tree);
        }

        delete[] data;
    }

    return isPTR;
}

// VERIFY: 45dcce86 auto
// OFFSET: 0x00402B20
void SetPaths() {
    SFile::DisableSFileCheckDisk();
    SFile::EnableDirectAccess(0);

    const char* path = CmdLineGetString(CMD_DATA_DIR);
    char buffer[260];

    if (!*path) {
        OsGetExePath(buffer, sizeof(buffer));
        path = buffer;
    }

    OsSetCurrentDirectory(path);
    SFile::SetBasePath(path);
    s_unk_B2FA10 = 0;
    SFile::SetDataPath(s_dataDirectories[0]);
}

// VERIFY: fb9a1306 auto
// OFFSET: 0x00402A50
const char* GetInstallLocationString() {
    if (!SFile::IsTrial()) {
        return nullptr;
    }

    if (!s_installLocation[0]) {
        if (IsPTRBuild()) {
            SRegLoadString("World of Warcraft\\PTR", "RetailInstallPath", 1, s_installLocation, sizeof(s_installLocation));
            return s_installLocation;
        }

        SRegLoadString("World of Warcraft", "RetailInstallPath", 1, s_installLocation, sizeof(s_installLocation));
    }

    return s_installLocation;
}

// VERIFY: 1ebb13ac auto
// OFFSET: 0x00402AA0
static bool BuildDataFileName(int32_t directory, char* buffer, uint32_t size, const char* pattern, const char* locale) {
    if (directory == 2) {
        if (!GetInstallLocationString()) {
            return false;
        }

        SStrPrintf(buffer, size, "%s%s%s", GetInstallLocationString(), s_dataDirectories[2], pattern);
    } else {
        SStrPrintf(buffer, size, "%s%s", s_dataDirectories[directory], pattern);
    }

    for (char* token = SStrStr(buffer, "****"); token; token = SStrStr(buffer, "****")) {
        memcpy(token, locale, 4);
    }

    return true;
}

// OFFSET: 0x00402B90
static bool CheckDataFilesExist() {
    char installPath[1024] = {};
    char dataPath[1024];
    char parentPath[1024];

    BuildDataFileName(0, dataPath, sizeof(dataPath), s_archives[17].m_name, "----");
    BuildDataFileName(1, parentPath, sizeof(parentPath), s_archives[17].m_name, "----");

    if (BuildDataFileName(2, installPath, sizeof(installPath), s_archives[17].m_name, "----")) {
        return OsFileExists(dataPath) || OsFileExists(parentPath) || OsFileExists(installPath);
    }

    return OsFileExists(dataPath) || OsFileExists(parentPath);
}

// OFFSET: 0x00402D50
static void CheckAvailableLocales(char* locale) {
    if (!CheckDataFilesExist()) {
        return;
    }

    for (int32_t i = 0; i < 12; i++) {
        char path[1024];
        g_hasIsoLocale[i] = false;

        for (int32_t directory = 0; directory < 3; directory++) {
            if (BuildDataFileName(directory, path, sizeof(path), s_archives[19].m_name, g_localeNames[i]) && OsFileExists(path)) {
                g_hasIsoLocale[i] = true;
                break;
            }
        }
    }

    int32_t index = LocaleNameToIndex(locale);

    for (int32_t i = 0; i < 12 && !g_hasIsoLocale[index]; i++) {
        index = (index + 1) % 12;
    }

    SStrCopy(locale, g_localeNames[index], STORM_MAX_STR);
}
// OFFSET: 0x00402F60
static void LoadMopaqFileSigningData(const char* locale) {
    SFileClearAllFileFlags();

    char* data;
    uint32_t size;

    if (!SFile::LoadFile("signaturefile", reinterpret_cast<void**>(&data), &size, 0, nullptr)) {
        SErrPrepareAppFatal(__FILE__, __LINE__);
        SErrDisplayAppFatalCustom(2, "file: %s", "signaturefile");
    }

    SFileSetSignatureCallbacks(1);

    if (!SSignatureVerify(reinterpret_cast<const uint8_t*>(data), size, s_signatureModulus, sizeof(s_signatureModulus), s_signatureExponent, sizeof(s_signatureExponent))) {
        SErrPrepareAppFatal(__FILE__, __LINE__);
        SErrDisplayAppFatalCustom(0x4DC, "Could not authenticate the %s file.", "signaturefile");
    }

    char* line = SStrStr(data, "\r\n");
    *line = '\0';
    int32_t version = SStrToInt(data);

    char* signature = line + 2;
    line = SStrStr(signature, "\r\n");
    *line = '\0';
    int32_t signatureVersion = atol(signature);

    if (signatureVersion != 2) {
        SErrPrepareAppFatal(__FILE__, __LINE__);
        SErrDisplayAppFatalCustom(0x85100083, "Signature string (%s) in %s file does not match game's signature version (%d).", signature, "signaturefile", signatureVersion);
    }

    char* scope = line + 2;
    line = SStrStr(scope, "\r\n");
    *line = '\0';

    if (SStrCmp(scope, "RELEASE", 7)) {
        SErrPrepareAppFatal(__FILE__, __LINE__);
        SErrDisplayAppFatalCustom(0x85100083, "Illegal scope string %s in %s: must be \"RELEASE\".", scope, "signaturefile");
    }

    char* record = line + 2;

    if (version != 2) {
        SErrPrepareAppFatal(__FILE__, __LINE__);
        SErrDisplayAppFatalCustom(0x85100083, "Format version in %s file is not supported.", "signaturefile");
        SFile::Unload(data);
        return;
    }

    bool expansion = s_expansionLevel >= 1;
    uint32_t localeTag = *reinterpret_cast<const uint32_t*>(locale);

    while (record < data + size && SStrCmp(record, "NGIS", 4)) {
        char* hash = record + 7;
        char* name = hash + 0x21;
        record[4] = '\0';
        record[6] = '\0';
        hash[0x20] = '\0';

        char* end = SStrStr(name, "\r\n");
        *end = '\0';

        uint32_t tag = *reinterpret_cast<const uint32_t*>(record);

        if ((tag == 'esab' || tag == localeTag) && (record[5] == 'c' || (record[5] == 'e' && expansion))) {
            MD5 md5(hash);
            SFileSetFileBlockEntryMD5(name, md5);
        }

        record = end + 2;
    }

    SFile::Unload(data);
}
// OFFSET: 0x00403200
static bool TimingMethodCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t method = atol(value);

    if (method > 2) {
        ConsolePrintf("'%s' is not a valid timing method. Valid methods are:", value);

        for (int32_t i = 0; i <= 2; i++) {
            ConsolePrintf("  %d - %s", i, GetTimingMethodName(static_cast<OsTimeManager::TimingMethod>(i)));
        }

        return false;
    }

    if (oldValue && method != static_cast<uint32_t>(atol(oldValue))) {
        CVar* testError = static_cast<CVar*>(arg);

        if (testError->m_intValue) {
            testError->SetReadOnly(false);
            testError->Set("0", true, false, false, true);
            testError->SetReadOnly(true);
        }
    }

    return true;
}

// VERIFY: 36ea6d32 auto
// OFFSET: 0x004032A0
static int32_t CCommand_TimingInfo(const char* command, const char* arguments) {
    int32_t desired = CVar::LookupRegistered("timingMethod")->m_intValue;
    OsTimeManager::TimingMethod selected = OsTimeGetTimingMethod();
    int32_t testError = CVar::LookupRegistered("timingTestError")->m_intValue;

    ConsolePrintf("Timing method desired: %d - %s", desired, GetTimingMethodName(static_cast<OsTimeManager::TimingMethod>(desired)));
    ConsolePrintf("Timing method selected: %d - %s", selected, GetTimingMethodName(selected));
    ConsolePrintf("Timing test error: %d", testError);

    return 1;
}

// VERIFY: c92cfd5d auto
// OFFSET: 0x00403300
int ForEveryRunOnceWTF::FileNameCompare(const void* a, const void* b) {
    return SStrSortUTF8I(*static_cast<char* const*>(a), *static_cast<char* const*>(b), STORM_MAX_STR);
}

// VERIFY: 52f0e60d auto
// OFFSET: 0x00403320
static bool LocaleCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    SStrCopy(s_localeCode, value, sizeof(s_localeCode));

    return true;
}

// OFFSET: 0x004033E0
static void LogRealmInfo(char* buffer, uint32_t size) {
    RealmInfo* realm = ClientServices::GetSelectedRealm();

    if (realm && realm->m_realmName[0] && realm->m_realmAddress[0]) {
        SStrPack(buffer, "Realm: ", size);
        SStrPack(buffer, realm->m_realmName, size);
        SStrPack(buffer, " [", size);
        SStrPack(buffer, realm->m_realmAddress, size);
        SStrPack(buffer, "]", size);
    } else {
        SStrPack(buffer, "Realm: ???", size);
    }

    SStrPack(buffer, "\r\n", size);
}

// OFFSET: 0x00403460
static void LogObjectInfo(CGObject_C* object, const char* label, char* buffer, uint32_t size) {
    if (!SMemIsValidPointer(object, sizeof(CGObject_C), 0)) {
        return;
    }

    C3Vector position = object->GetPosition();
    WOWGUID guid = static_cast<WOWGUID>(object->m_obj->m_guid.m_guidHigh) << 32 | object->m_obj->m_guid.m_guidLow;

    char line[260];
    SStrPrintf(line, sizeof(line), "%s: %s, %016I64X, (%g,%g,%g)\r\n", label, object->GetObjectName(), guid, position.x, position.y, position.z);
    SStrPack(buffer, line, size);
}

// VERIFY: bf3a602d auto
// OFFSET: 0x00403560
static void SetInstallPath() {
    const char* key = "World of Warcraft";

    if (IsPTRBuild()) {
        key = "World of Warcraft\\PTR";
    }

    if (SFile::IsTrial()) {
        key = "World of Warcraft Trial";
    }

    char path[1560];
    GetExecutablePathUTF8(path, sizeof(path));

    if (!path[0]) {
        return;
    }

    char* slash = SStrChrR(path, '\\');

    if (!slash) {
        return;
    }

    slash[1] = '\0';

    if (SFile::IsTrial()) {
        SRegSaveString(key, "InstallPath", 1, path);
    } else {
        SRegSaveString(key, "InstallPath", 4, path);
    }
}

// OFFSET: 0x00403600
static void LoadMFIL(const char* file) {
    Blizzard::Time::GetElapsedSinceStart(true);

    if (OsFileExists(file) && file) {
        s_streamingDiag |= 1;
        int32_t initialized = SFile::InitializeStreaming(file);
        OsDeleteFile("WoW.stor");

        if (initialized) {
            s_streamingDiag |= 2;
        } else {
            SStrCopy(s_streamingError, SFile::GetErrorDetails(), 2048);
        }
    }

    if (SFile::IsTrial()) {
        RegisterOggDecompressor();
    }

    SFile::SetStreamingStatus(s_streamingDiag & 1, (s_streamingDiag >> 1) & 1, (s_streamingDiag >> 2) & 1, (s_streamingDiag >> 3) & 1, 12340);
}

// VERIFY: 12e55922 auto
// OFFSET: 0x00404930
static bool OpenArchiveWithRetry(const char* name, int32_t priority, uint32_t slot, const char* locale) {
    for (int32_t directory = 0; directory < 3; directory++) {
        char path[260];

        if (!BuildDataFileName(directory, path, sizeof(path), name, locale)) {
            continue;
        }

        if (SFile::OpenArchive(path, priority, 0xC00, &s_archiveHandles[slot])) {
            return true;
        }

        s_archiveHandles[slot] = nullptr;

        switch (SErrGetLastError()) {
            case 5:
            case 38:
            case 108:
            case 131:
            case 0x8510006C:
            case 0x85100083:
                return false;
        }
    }

    return false;
}

// OFFSET: 0x00404B00
static void LogZoneInfo(CGObject_C* object, char* buffer, uint32_t size) {
    C3Vector position = object->GetPosition();
    SStrPack(buffer, "Local Zone: ", size);

    uint32_t areaId = 0;
    CMap::QueryAreaId(object->m_worldObject, areaId);

    char name[260];
    AreaListGetName(areaId, name, sizeof(name), 1);
    SStrPack(buffer, name, size);
    SStrPack(buffer, "\r\n", size);
}

// OFFSET: 0x00404B80
int32_t STORMAPI WowLogHeader(char* buffer, uint32_t bufferchars) {
    SStrPrintf(buffer, bufferchars, "WoWBuild: %d\r\n", 12340);

    if (ClntObjMgrIsValid(0)) {
        CGObject_C* player = ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__);

        if (player && SMemIsValidPointer(player, sizeof(CGPlayer_C), 0)) {
            LogRealmInfo(buffer, bufferchars);
            LogZoneInfo(player, buffer, bufferchars);
            LogObjectInfo(player, "Local Player", buffer, bufferchars);

            CGObject_C* object = ClntObjMgrObjectPtr(static_cast<CGPlayer_C*>(player)->m_dataA20, TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (object) {
                LogObjectInfo(object, "Attacking Target", buffer, bufferchars);
            }

            object = ClntObjMgrObjectPtr(CGGameUI::m_currentObjectTrack, TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (object) {
                LogObjectInfo(object, "Current Object Track", buffer, bufferchars);
            }

            object = ClntObjMgrObjectPtr(CGGameUI::m_interactTarget, TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (object) {
                LogObjectInfo(object, "Interact Target", buffer, bufferchars);
            }

            object = ClntObjMgrObjectPtr(CGGameUI::m_lockedTarget, TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (object) {
                LogObjectInfo(object, "Locked Target", buffer, bufferchars);
            }

            object = ClntObjMgrObjectPtr(CGGameUI::m_lastEnemyTarget, TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (object) {
                LogObjectInfo(object, "Last Enemy Target", buffer, bufferchars);
            }

            object = ClntObjMgrObjectPtr(CGGameUI::GetCursorItem(), TYPEMASK_ITEM, __FILE__, __LINE__);

            if (object) {
                LogObjectInfo(object, "Cursor Item", buffer, bufferchars);
            }

            char line[1024];
            line[0] = '\0';

            SStrPrintf(line, sizeof(line), "Total lua memory: %dKB\n", FrameScript_GetTotalMemoryUsed());
            SStrPack(buffer, line, bufferchars);

            SStrPrintf(line, sizeof(line), "Current Addon: %s\n", lua_tainted);
            SStrPack(buffer, line, bufferchars);

            char function[1024];
            FrameScript_GetCurrentFunction(FrameScript_GetContext(), function, sizeof(function));
            SStrPrintf(line, sizeof(line), "Current Addon function: %s\n", function);
            SStrPack(buffer, line, bufferchars);

            SStrPrintf(line, sizeof(line), "Current Addon object: %s\n", FrameScript_GetCurrentObject(FrameScript_GetContext(), 2));
            SStrPack(buffer, line, bufferchars);

            if (GetNumAddOns()) {
                SStrPack(buffer, "Add Ons: ", bufferchars);

                for (uint32_t i = 0; i < GetNumAddOns(); i++) {
                    const char* name = GetAddOnByIndex(i);

                    if (IsAddOnLoading(name)) {
                        SStrPack(buffer, name, bufferchars);
                        SStrPack(buffer, " ", bufferchars);
                    }
                }

                SStrPack(buffer, "\r\n", bufferchars);
            }
        }
    }

    SStrPack(buffer, "Settings: \r\n", bufferchars);
    CVar::LogAll(buffer, bufferchars, 0, 64);
    GxLogCrashInfo(buffer, bufferchars);

    return 1;
}

// OFFSET: 0x00405DD0
static void OpenArchives() {
    int32_t availability = CheckDataFilesExist() + 1;

    for (uint32_t i = 0; i < sizeof(s_archives) / sizeof(s_archives[0]); i++) {
        s_archives[i].m_opened = false;
    }

    const char* locale = (availability & 2) ? CVar::Lookup("locale")->m_stringValue.m_str : "----";

    PatchFiles patches(locale, false);

    int32_t priority = 64;
    uint32_t slot = 0;
    s_archiveHandles.SetCount(64);

    for (uint32_t i = patches.m_files.Count(); i; i--) {
        if (SFile::OpenArchive(patches.m_files[i - 1], priority, 0xC00, &s_archiveHandles[slot])) {
            if (SStrStrI(patches.m_files[i - 1], "Start.MPQ")) {
                s_unk_AB6164 = priority;
            }

            slot++;
            priority++;
        }
    }

    uint32_t patchCount = slot;

    for (uint32_t i = 0; i < sizeof(s_archives) / sizeof(s_archives[0]); i++) {
        if (s_archives[i].m_unk04 == 3 && OpenArchiveWithRetry(s_archives[i].m_name, priority, slot, locale)) {
            slot++;
            s_archives[i].m_opened = true;
            priority++;
        }
    }

    for (uint32_t patch = 0; patch < patchCount; patch++) {
        SArchive* patchArchive = s_archiveHandles[patch];

        for (uint32_t i = 0; i < sizeof(s_archives) / sizeof(s_archives[0]); i++) {
            if (s_archives[i].m_unk04 != 3) {
                continue;
            }

            if (!s_archives[i].m_opened && SStrCmpI(locale, "zhCN", STORM_MAX_STR) && SStrCmpI(locale, "enCN", STORM_MAX_STR) && SStrCmpI(locale, "zhTW", STORM_MAX_STR) && SStrCmpI(locale, "enTW", STORM_MAX_STR)) {
                continue;
            }

            char path[256];
            SStrCopy(path, s_archives[i].m_name, sizeof(path));
            path[SStrLen(path) - 4] = '\0';

            if (SFile::OpenPathAsArchive(patchArchive, path, priority, 0, &s_archiveHandles[slot])) {
                slot++;
                priority++;
            }
        }
    }

    s_numPatchArchives = slot;
    int32_t basePriority = 63;
    s_firstBaseArchive = slot;

    for (uint32_t i = 0; i < sizeof(s_archives) / sizeof(s_archives[0]); i++) {
        s_archiveHandles[slot] = nullptr;

        if (s_archives[i].m_unk04 == 3 || s_archives[i].m_unk04 == 4) {
            slot++;
            continue;
        }

        bool optional = s_archives[i].m_unk04 == 2;
        bool available = (availability & s_archives[i].m_unk08) != 0;
        bool allowed = !SFile::IsTrial() || s_archives[i].m_unk04 != 2;

        if (allowed && available) {
            if (!OpenArchiveWithRetry(s_archives[i].m_name, basePriority, slot, locale) && !optional) {
                char path[260];
                SStrPrintf(path, sizeof(path), "%s%s", s_dataDirectories[0], s_archives[i].m_name);

                for (char* token = SStrStr(path, "****"); token; token = SStrStr(path, "****")) {
                    memcpy(token, locale, 4);
                }

                const char* details = SFile::GetErrorDetails();
                char errorText[256];
                SErrGetErrorStr(SErrGetLastError(), errorText, sizeof(errorText));

                if (s_streamingDiag & 1) {
                    SErrDisplayAppFatal("Failed to read data from the network. Please check your Internet connection and try again.\n\nDebug Details:\n\n%s%sStorm Error Msg:%s", details, s_streamingError, errorText);
                }

                char message[256];
                SStrPrintf(message, sizeof(message), "Failed to open archive %s.", s_archives[i].m_name);

                char title[256];
                SStrCopy(title, "Missing or corrupted data", STORM_MAX_STR);
                OsGuiMessageBox(OsGuiGetWindow(2), 0, message, title);
                StormExit(SErrGetLastError());
            }
        }

        slot++;
        basePriority--;
        s_archives[i].m_opened = true;
    }

    if (SFile::IsTrial()) {
        basePriority = 64;
    }

    if (SFile::IsStreamingMode()) {
        for (uint32_t i = 0; i < sizeof(s_archives) / sizeof(s_archives[0]); i++) {
            if (s_archives[i].m_unk04 == 4 && OpenArchiveWithRetry(s_archives[i].m_name, basePriority, slot, locale)) {
                s_archives[i].m_opened = true;
                slot++;
            }
        }
    }

    s_archiveHandles.SetCount(slot);

    if (s_archiveHandles[s_firstBaseArchive + 16]) {
        s_expansionLevel = 2;
    } else {
        s_expansionLevel = s_archiveHandles[s_firstBaseArchive + 15] != nullptr;
    }

    void* profile = ProfileCreate();

    if (ProfileReadFile(profile, "Wow.ini")) {
        ProfileGetValue(profile, "WoW Config", "Region", &s_region, 0);

        char language[16];
        char country[16];

        if (ProfileGetValue(profile, "WoW Config", "Language", language, sizeof(language), 0) && ProfileGetValue(profile, "WoW Config", "Country", country, sizeof(country), 0)) {
            SStrPrintf(s_iniLocale, sizeof(s_iniLocale), "%s%s", language, country);

            uint32_t code = 0;

            for (uint32_t i = 0; i < 4 && s_iniLocale[i]; i++) {
                code = (code << 8) + s_iniLocale[i];
            }

            SFile::s_locale = code;
            bool isGB = SStrCmpI("enGB", s_iniLocale, STORM_MAX_STR) == 0;

            for (int32_t i = 1; i < 9; i++) {
                if (!SStrCmpI(s_iniLocale, s_iniLocaleNames[i - 1], STORM_MAX_STR) || (isGB && !SStrCmpI("enUS", s_iniLocaleNames[i - 1], STORM_MAX_STR))) {
                    s_locale = i;

                    switch (i) {
                        case 1:
                        case 2:
                        case 4:
                        case 5:
                            FrameScript_SetPluralRule(PLURAL_RULE_1);
                            break;

                        case 8:
                            FrameScript_SetPluralRule(PLURAL_RULE_2);
                            break;

                        default:
                            FrameScript_SetPluralRule(PLURAL_RULE_0);
                            break;
                    }

                    if (availability & 1) {
                        CVar::Lookup("locale")->Set(s_iniLocale, true, false, false, true);
                        locale = s_iniLocale;
                    }

                    break;
                }
            }
        }
    }

    ProfileClose(profile);
    ClientServices::InitLoginServerCVars(1, locale);
    LoadMopaqFileSigningData(locale);
}
// OFFSET: 0x004064A0
int32_t ForEveryRunOnceWTF::AppendFileName(OS_FILE_DATA& data, void* param) {
    TSGrowableArray<char*>* files = static_cast<TSGrowableArray<char*>*>(param);
    char** slot = files->New();
    *slot = SStrDupA(data.fileName, __FILE__, __LINE__);

    return 0;
}

// OFFSET: 0x00406740
void ForEveryRunOnceWTF::Execute(int32_t (*callback)(const char*)) {
    char basePath[260];
    SFile::GetBasePath(basePath, sizeof(basePath));

    char dir[260];
    SStrPrintf(dir, sizeof(dir), "%sWTF\\", basePath);
    OsFileList(dir, "RunOnce*.wtf", ForEveryRunOnceWTF::AppendFileName, &this->m_files, 1);

    uint32_t count = this->m_files.Count();
    qsort(this->m_files.Ptr(), count, sizeof(char*), ForEveryRunOnceWTF::FileNameCompare);

    for (int32_t i = 0; i < static_cast<int32_t>(count); i++) {
        callback(this->m_files[i]);
        SMemFree(this->m_files[i], __FILE__, __LINE__, 0);
    }
}

// OFFSET: 0x004067F0
bool InitializeGlobal() {
    SCmdRegisterArgList(s_wowArgList, sizeof(s_wowArgList) / sizeof(s_wowArgList[0]));
    CmdLineProcess();
    LoadMFIL("WoW.mfil");

    if (s_unk_B2FA10 != 2) {
        SetInstallPath();
    }

    bool firstRun = false;

    if (SFile::IsTrial()) {
        firstRun = !OsDirectoryExists("WTF/Account");
    }

    ClientServices::LoadCDKey();
    ConsoleInitializeClientCommand();
    CVar::Initialize("Config.wtf");
    CVar::ArchiveCodeRegisteredOnly();

    {
        ForEveryRunOnceWTF runOnce;
        runOnce.Execute(static_cast<int32_t (*)(const char*)>(CVar::Load));
    }

    CVar::Register("dbCompress", "Database compression", 0, "-1", nullptr, CATEGORY_DEFAULT, false, nullptr, false);
    CVar* locale = CVar::Register("locale", "Set the game locale", 0, "****", LocaleCallback, CATEGORY_DEFAULT, false, nullptr, false);

    if (!SStrCmp(locale->m_stringValue.m_str, "****", STORM_MAX_STR)) {
        locale->Set("enUS", true, false, false, true);
    }

    CVar::Register("useEnglishAudio", "override the locale and use English audio", 0, "0", nullptr, CATEGORY_DEFAULT, false, nullptr, false);

    if (SFile::IsTrial()) {
        StartupReport(firstRun);
    }

    char localeName[5];
    SStrCopy(localeName, locale->m_stringValue.m_str, sizeof(localeName));
    CheckAvailableLocales(localeName);
    locale->Set(localeName, true, false, false, true);

    char dataPath[260];
    SStrPrintf(dataPath, sizeof(dataPath), "%s%s", s_dataDirectories[0], locale->m_stringValue.m_str);
    SFile::SetDataPathAlternate(dataPath);
    SFile::RebuildHash();
    OpenArchives();

    CVar* affinity = CVar::Register("processAffinityMask", "Sets which core(s) WoW may execute on - changes require restart to take effect", 2, "0", ProcessAffinityMaskCallback, CATEGORY_DEBUG, false, nullptr, false);
    CVar* videoOptionsVersion = CVar::Lookup("videoOptionsVersion");

    if (!videoOptionsVersion || videoOptionsVersion->m_intValue < 3) {
        char value[8];
        SStrPrintf(value, sizeof(value), "%u", 0);
        affinity->Set(value, true, false, false, true);
        affinity->Update();
    }

    uint32_t mask = affinity->m_intValue;

    if (mask) {
        SSetCurrentProcessAffinityMask(mask);
    }

    BaseInitializeGlobal();
    EventInitialize(1, 0);

    CVar* timingTestError = CVar::Register("timingTestError", "Error reported by the timing validation system", 6, "0", nullptr, CATEGORY_DEFAULT, false, nullptr, false);
    CVar* timingMethod = CVar::Register("timingMethod", "Desired method for game timing", 2, "0", TimingMethodCallback, CATEGORY_DEFAULT, false, timingTestError, false);
    OsTimeStartup(static_cast<OsTimeManager::TimingMethod>(timingMethod->m_intValue));
    ConsoleCommandRegister("timingInfo", CCommand_TimingInfo, CATEGORY_DEBUG, nullptr);

    s_timeTestError = OsTimeGetTestError();

    if (s_timeTestError != static_cast<uint32_t>(timingTestError->m_intValue)) {
        char buffer[16];
        sprintf(buffer, "%d", s_timeTestError);
        timingTestError->SetReadOnly(false);
        timingTestError->Set(buffer, true, false, false, true);
        timingTestError->Update();
        timingTestError->SetReadOnly(true);
        ConsolePrintf("Timing test error: %d", timingTestError);
    }

    g_startup_StringsDB.Load(__FILE__, __LINE__);

    Startup_StringsRec* title = g_startup_StringsDB.GetRecord(1);
    char windowTitle[260];
    SStrCopy(windowTitle, title ? title->m_message : "World of Warcraft", STORM_MAX_STR);
    ConsoleDeviceInitialize(windowTitle, 1, 1);

    OsIMEInitialize();
    g_rndSeed.SetSeed(OsGetAsyncTimeMs());
    g_clientEventContext = EventCreateContextEx(1, InitializeEngineCallback, DestroyEngineCallback, 0, 0);

    return true;
}

// OFFSET: 0x00406C70
int32_t CommonMain() {
    StormInitialize();
    SErrCatchUnhandledExceptions();
    CreateNamedEventA("Blizzard Entertainment World of Warcraft", 0);

    int32_t option = 1;
    StormSetOption(10, &option, sizeof(option));
    option = 1;
    StormSetOption(11, &option, sizeof(option));

    OsSystemEnableCpuLog();
    SetPaths();

    uint32_t sendErrorLogs = 1;

    if (!SRegLoadValue("World of Warcraft\\Client", "SendErrorLogs", 0, &sendErrorLogs)) {
        sendErrorLogs = 1;
        SRegSaveValue("World of Warcraft\\Client", "SendErrorLogs", 0, 1);
    }

    SErrSetLogTitleString("World of WarCraft (build 12340)");
    SErrSetLogCallback(WowLogHeader);

    if (sendErrorLogs) {
        SErrRegisterHandler(SendErrorLog);
    }

    if (InitializeGlobal()) {
        EventDoMessageLoop();
        BeginCloseGame();
    }

    StormDestroy();

    if (SFile::IsTrial()) {
        ShutdownFileCache();
    }

    Blizzard::Time::GetElapsedSinceStart(false);

    if (SFile::IsTrial()) {
        StartupReportSendTelemetry(false, false);
    }

    return 0;
}
