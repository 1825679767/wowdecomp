#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFile.h"

// STATIC: 0x00B38344
Blizzard::Thread::LocalObject<bool> System_SFile::s_retryNativeRead;

void STORMAPI SFile::CreateOverlapped(SOVERLAPPED* overlapped) {
}

void STORMAPI SFile::DestroyOverlapped(SOVERLAPPED* overlapped) {
}
