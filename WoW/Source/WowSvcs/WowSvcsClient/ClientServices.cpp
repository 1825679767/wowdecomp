#include <cstdint>
#include <cstring>
#include "ClientServices.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/Types.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00C79B20
static CLIENTCDKEY s_cdKey;

// STATIC: 0x00C79CEA
static bool s_cdKeyLoaded;

// STATIC: 0x00C79CF8
static CVar* s_realmListBNVar;

// STATIC: 0x00C79CFC
static CVar* s_portalVar;

// STATIC: 0x00C79D00
static CVar* s_realmListVar;

// STATIC: 0x00C79D04
static CVar* s_serverAlertVar;

// STATIC: 0x00C79D0C
static CVar* s_decorateAccountName;

// VERIFY: b1444fa0 auto
// OFFSET: 0x006B0190
void ClientServices::LoadCDKey() {
    memset(&s_cdKey, 0, sizeof(s_cdKey));

    SArchive* archive;

    if (!SFile::OpenArchive("Data\\base.MPQ", 100, 0x800, &archive) || !archive) {
        return;
    }

    void* data;
    uint32_t size = sizeof(s_cdKey.m_key);

    if (SFile::ReadFile(archive, "telemetry.dat", &data, &size, 0, 0, nullptr) && data) {
        if (size == sizeof(s_cdKey.m_key)) {
            s_cdKeyLoaded = true;
            s_cdKey.m_length = 20;
            memcpy(s_cdKey.m_key, data, sizeof(s_cdKey.m_key));
        }

        SFile::Unload(data);
    }

    SFile::CloseArchive(archive);
}

// VERIFY: c63b3523 auto
// OFFSET: 0x006B0BF0
void ClientServices::InitLoginServerCVars(int32_t overwrite, const char* locale) {
    if (!s_realmListBNVar || !s_realmListVar || overwrite) {
        s_decorateAccountName = CVar::Register("decorateAccountName", "", 0, "0", nullptr, CATEGORY_NET, false, nullptr, false);
    }

    char localeDir[260];

    if (locale && *locale) {
        SStrPrintf(localeDir, sizeof(localeDir), "data\\%s\\", locale);
    } else {
        localeDir[0] = '\0';
    }

    char path[260];

    if (!s_realmListBNVar || overwrite) {
        s_realmListBNVar = CVar::Register("realmListbn", "Address of Battle.net server", 0, "", nullptr, CATEGORY_NET, false, nullptr, false);
        SStrPrintf(path, sizeof(path), "%srealmlistbn.wtf", localeDir);

        if (!CVar::Load(path)) {
            CVar::Load("realmlistbn.wtf");
        }
    }

    if (!s_portalVar || overwrite) {
        s_portalVar = CVar::Register("portal", "Name of Battle.net portal to use", 0, "", nullptr, CATEGORY_NET, false, nullptr, false);
    }

    if (!s_serverAlertVar || overwrite) {
        s_serverAlertVar = CVar::Register("serverAlert", "Get the glue-string tag for the URL", 0, "SERVER_ALERT_URL", nullptr, CATEGORY_NET, false, nullptr, false);
    }

    if (!s_realmListVar || overwrite) {
        s_realmListVar = CVar::Register("realmList", "Address of realm list server", 0, "us.logon.worldofwarcraft.com:3724", nullptr, CATEGORY_NET, false, nullptr, false);
        SStrPrintf(path, sizeof(path), "%srealmlist.wtf", localeDir);

        if (!CVar::Load(path)) {
            CVar::Load("realmlist.wtf");
        }
    }
}
