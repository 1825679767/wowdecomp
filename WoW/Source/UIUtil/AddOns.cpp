#include "AddOns.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Storm/H/storm.h"

// VERIFY: 6c5d7816 auto
// OFFSET: 0x005F4CE0
static int32_t DeleteInterfaceAddOns(OS_FILE_DATA& data, void* param) {
    char path[260];
    SStrPrintf(path, sizeof(path), "%s%s\\%s.toc", "Interface\\AddOns\\", data.fileName, data.fileName);

    if (OsFileExists(path)) {
        SStrPrintf(path, sizeof(path), "%s%s", "Interface\\AddOns\\", data.fileName);

        char oldPath[260];
        SStrPrintf(oldPath, sizeof(oldPath), "%s.old", path);
        OsRemoveDirectoryRecurse(oldPath, 1);
        OsMoveFile(path, oldPath);
        *static_cast<bool*>(param) = true;
    }

    return 0;
}

// VERIFY: 012df8c8 auto
// OFFSET: 0x005F4D90
void DeleteInterfaceFiles() {
    bool deleted = false;
    const char* directories[] = { "Interface\\GlueXML", "Interface\\FrameXML" };

    for (uint32_t i = 0; i < sizeof(directories) / sizeof(directories[0]); i++) {
        const char* directory = directories[i];

        if (OsDirectoryExists(directory)) {
            char oldPath[260];
            SStrPrintf(oldPath, sizeof(oldPath), "%s.old", directory);
            OsRemoveDirectoryRecurse(oldPath, 1);
            OsMoveFile(directory, oldPath);
            deleted = true;
        }
    }

    OsFileList("Interface\\AddOns\\", "Blizzard*", DeleteInterfaceAddOns, &deleted, 0);

    if (deleted) {
        SFile::RebuildHash();
    }
}
