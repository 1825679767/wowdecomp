#include <cstdint>
#include "Client.h"
#include "Engine/Source/Base/CmdLine.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Os/Path.h"

// STATIC: 0x00AB6158
static const char* s_dataDirectories[3] = { "Data\\", "..\\Data\\", "Data\\" };

// STATIC: 0x00AB6350
WowClientDB<Startup_StringsRec> g_startup_StringsDB;

// STATIC: 0x00B2FA10
static uint32_t s_unk_B2FA10;

// STATIC: 0x00C5DE9C
int32_t s_locale;

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
