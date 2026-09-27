#include "ClientServices.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/Types.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00C79CF8
static CVar* s_realmListBNVar;

// STATIC: 0x00C79CFC
static CVar* s_portalVar;

// STATIC: 0x00C79D00
static CVar* s_realmListVar;

// STATIC: 0x00C79D04
CVar* ClientServices::s_serverAlertVar;

// STATIC: 0x00C79D08
CVar* ClientServices::s_realmNameVar;

// STATIC: 0x00C79D0C
static CVar* s_decorateAccountName;

// VERIFY: 18091757 auto
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

    if (!ClientServices::s_serverAlertVar || overwrite) {
        ClientServices::s_serverAlertVar = CVar::Register("serverAlert", "Get the glue-string tag for the URL", 0, "SERVER_ALERT_URL", nullptr, CATEGORY_NET, false, nullptr, false);
    }

    if (!s_realmListVar || overwrite) {
        s_realmListVar = CVar::Register("realmList", "Address of realm list server", 0, "us.logon.worldofwarcraft.com:3724", nullptr, CATEGORY_NET, false, nullptr, false);
        SStrPrintf(path, sizeof(path), "%srealmlist.wtf", localeDir);

        if (!CVar::Load(path)) {
            CVar::Load("realmlist.wtf");
        }
    }
}
