#include "MapObjRead.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"

// STATIC: 0x00ADFC4C
TSList<CMapObj, TSGetExplicitLink<CMapObj> > CMapObj::s_asyncLoadQueue = TSExplicitList<CMapObj, offsetof(CMapObj, m_asyncLoadLink)>();

// STATIC: 0x00ADFC58
TSList<CMapObjGroup, TSGetExplicitLink<CMapObjGroup> > CMapObj::s_unk_ADFC58 = TSExplicitList<CMapObjGroup, offsetof(CMapObjGroup, m_asyncLoadLink)>();

// VERIFY: 737e6a62 manual
// OFFSET: 0x007D80C0
bool CMapObj::Read(char* fileName) {
    SStrCopy(this->m_wmoName, fileName, STORM_MAX_STR);

    SFileImpl* file = nullptr;

    if (CMap::SafeOpen(fileName, &file)) {
        SStrCopy(this->m_wmoName, fileName, STORM_MAX_STR);
    } else {
        CMap::SafeOpen("world\\wmo\\Dungeon\\test\\missingwmo.wmo", &file);
        SStrCopy(this->m_wmoName, "world\\wmo\\Dungeon\\test\\missingwmo.wmo", STORM_MAX_STR);
    }

    this->m_wmoFileSize = SFileGetFileSize(file, nullptr);
    this->m_pWmoData = SMemAlloc(this->m_wmoFileSize, __FILE__, __LINE__, 0);

    this->m_asyncObject = AsyncFileReadAllocObject();
    this->m_asyncObject->m_file = file;
    this->m_asyncObject->m_userArg = this;
    this->m_asyncObject->m_buffer = this->m_pWmoData;
    this->m_asyncObject->m_size = this->m_wmoFileSize;
    this->m_asyncObject->m_userPostloadCallback = reinterpret_cast<void*>(CMapObj::PostloadCallback);
    this->m_asyncObject->m_priority = 124;
    AsyncFileReadObject(this->m_asyncObject, 0);

    CMapObj::s_asyncLoadQueue.LinkToTail(this);

    return true;
}

// VERIFY: d48a58f4 manual
// OFFSET: 0x007D85E0
void CMapObj::ReadGroup(uint32_t index, bool preLoad) {
    CMapObjGroup* group = this->m_mapObjGroupArray[index];

    char fileName[256];
    strcpy(fileName, this->m_wmoName);
    SStrPrintf(strrchr(fileName, '.'), sizeof(fileName), "_%03d", index);
    strcat(fileName, ".wmo");

    SFileImpl* file = nullptr;
    CMap::SafeOpen(fileName, &file);

    group->m_fileSize = SFileGetFileSize(file, nullptr);
    group->m_filePtr = SMemAlloc(group->m_fileSize, __FILE__, __LINE__, 0);
    group->m_parent = this;

    if (preLoad) {
        CMap::SafeRead(fileName, file, group->m_filePtr, group->m_fileSize);
        group->Create();
        SFileCloseFile(file);
        return;
    }

    CAsyncObject* object = AsyncFileReadAllocObject();
    object->m_file = file;
    object->m_buffer = group->m_filePtr;
    object->m_size = group->m_fileSize;
    object->m_userArg = group;
    object->m_userPostloadCallback = reinterpret_cast<void*>(CMapObjGroup::AsyncPostloadCallback);
    group->m_asyncObjPtr = object;
    AsyncFileReadObject(object, 0);

    CMapObj::s_unk_ADFC58.LinkToTail(group);
}
