#include "MapArea.h"
#include <cstring>
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "WoW/Source/WorldClient/CMapBaseObjLink.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include "WoW/Source/WorldClient/Types.h"
#include "WoW/Source/WorldClient/World.h"

void CMapArea::Destroy() {
}

// VERIFY: 2c619432 manual
// OFFSET: 0x005EEB70
void CMapArea::Initialize() {
}

// VERIFY: 8d38ed01 manual
// OFFSET: 0x007D6810
void CMapArea::BatchChunks(const C2iVector& pos) {
    int32_t index = 16 * pos.y + pos.x;

    CMapChunk* chunks[4];
    chunks[0] = this->m_mapChunks[index];
    chunks[1] = this->m_mapChunks[index + 16];
    chunks[2] = this->m_mapChunks[16 * pos.y + 1 + pos.x];
    chunks[3] = this->m_mapChunks[16 * pos.y + 17 + pos.x];

    for (int32_t i = 0; i < 4; i++) {
        if (!chunks[i]) {
            return;
        }
    }

    CMapChunk* topLeft = chunks[0];
    CMapChunk* bottomLeft = chunks[1];
    CMapChunk* topRight = chunks[2];
    CMapChunk* bottomRight = chunks[3];

    int32_t mergeLeft = CMapChunk::CanMergeChunkLayers(bottomLeft, topLeft);
    int32_t mergeRight = CMapChunk::CanMergeChunkLayers(bottomRight, topRight);
    int32_t mergeTop = CMapChunk::CanMergeChunkLayers(topRight, topLeft);
    int32_t mergeBottom = CMapChunk::CanMergeChunkLayers(bottomRight, bottomLeft);

    int32_t vertical = mergeRight + mergeLeft;
    int32_t horizontal = mergeBottom + mergeTop;

    if (vertical || horizontal) {
        int32_t firstPartner = 2;
        int32_t secondStart = 1;
        int32_t firstMerge = mergeTop;
        int32_t secondMerge = mergeBottom;
        uint8_t orientation = 2;

        if (vertical > horizontal) {
            firstMerge = mergeLeft;
            firstPartner = 1;
            secondStart = 2;
            secondMerge = mergeRight;
            orientation = 1;
        }

        if (firstMerge) {
            CMapRenderChunk* renderChunk = CMap::AllocRenderChunk();
            CMapChunk* partner = chunks[firstPartner];
            topLeft->m_renderChunk = renderChunk;
            renderChunk->AddBatch(topLeft, partner, &topLeft->m_topLeftCoords, orientation);
            partner->m_renderChunk = topLeft->m_renderChunk;
        }

        if (secondMerge) {
            CMapRenderChunk* renderChunk = CMap::AllocRenderChunk();
            CMapChunk* first = chunks[secondStart];
            first->m_renderChunk = renderChunk;
            renderChunk->AddBatch(first, bottomRight, &first->m_topLeftCoords, orientation);
            bottomRight->m_renderChunk = first->m_renderChunk;
        }
    }

    for (int32_t i = 0; i < 4; i++) {
        CMapChunk* chunk = chunks[i];

        if (!chunk->m_renderChunk) {
            chunk->AllocRenderChunkAndBatch();
        }
    }
}

// VERIFY: bb694f33 manual
// OFFSET: 0x007D6980
void CMapArea::LoadTerrainTexture(CMapAreaTexture* areaTexture, int32_t textureId) {
    bool shaders = g_theGxDevicePtr->Caps().m_texTarget[1] && g_theGxDevicePtr->Caps().m_shaderTargets[0] && g_theGxDevicePtr->Caps().m_shaderTargets[4];

    uint8_t flags = 0;

    if (this->m_textureFlags) {
        flags = this->m_textureFlags[textureId];
    }

    if ((flags & 0x1) || !CMap::enableSpecularTerrain) {
        if (!(flags & 0x1) || shaders) {
            areaTexture->m_texture = CMap::LoadTexture(areaTexture->m_textureName);
        } else {
            CImVector color;
            color.b = 0x00;
            color.g = 0x00;
            color.r = 0x00;
            color.a = 0xFF;
            areaTexture->m_texture = TextureCreateSolid(color);
        }
    } else {
        char fileName[256];
        SStrCopy(fileName, areaTexture->m_textureName, STORM_MAX_STR);
        char* ext = SStrChrR(fileName, '.');
        SStrCopy(ext, "_s.blp", STORM_MAX_STR);
        areaTexture->m_texture = CMap::LoadTexture(fileName);
    }
}

// VERIFY: 4696cf2f manual
// OFFSET: 0x007D6B30
CMapChunk* CMapArea::PrepareChunk(int32_t chunkX, int32_t chunkY) {
    int32_t index = chunkX + 16 * chunkY;

    bool firstUse = true;

    if (this->m_chunkInfo[index].m_flags & 0x1) {
        firstUse = false;
    }

    this->m_chunkInfo[index].m_flags |= 0x1;

    CMapChunk* chunk = CMap::AllocMapChunk();
    CMapBaseObjLink* link = CMap::AllocBaseObjLink(chunk);
    link->m_ref = this;
    this->m_chunkLinkList.LinkToTail(link);

    chunk->m_aIndex.x = chunkX;
    chunk->m_flags = static_cast<MapObjFlag>(0);
    chunk->m_aIndex.y = chunkY;
    chunk->m_cOffset.x = chunkX + this->m_tileChunkIndex.x;
    chunk->m_cOffset.y = chunkY + this->m_tileChunkIndex.y;
    chunk->m_sOffset.x = 8 * (chunkX + this->m_tileChunkIndex.x);
    chunk->m_sOffset.y = 8 * (chunkY + this->m_tileChunkIndex.y);
    chunk->Create(reinterpret_cast<SIffChunk*>(static_cast<uint8_t*>(this->m_fileBuffer) + this->m_chunkInfo[index].m_offset), firstUse);

    return chunk;
}

// VERIFY: 0ce80d37 manual
// OFFSET: 0x007D6BF0
void CMapArea::Update(int32_t addToScene, CiRect* chunkRect) {
    for (int32_t chunkY = chunkRect->minY; chunkY <= chunkRect->maxY; chunkY++) {
        int32_t localY = chunkY & 0xF;

        for (int32_t chunkX = chunkRect->minX; chunkX <= chunkRect->maxX; chunkX++) {
            int32_t localX = chunkX & 0xF;

            if (chunkX >= CWorld::gbChunkRect.minX && chunkX <= CWorld::gbChunkRect.maxX && chunkY >= CWorld::gbChunkRect.minY && chunkY <= CWorld::gbChunkRect.maxY) {
                CMapChunk** slot = &this->m_mapChunks[localX + 16 * localY];
                CMapChunk* chunk = *slot;

                if (!chunk) {
                    this->PrepareChunk(localX, localY);
                    chunk = *slot;
                }

                if (addToScene && CWorldScene::InsideFrustumRect(chunkRect)) {
                    CWorldScene::AddMapChunk(chunk);
                    chunk->CullAndBucketChunk();
                }
            } else {
                CMapChunk* chunk = this->m_mapChunks[16 * localY + localX];

                if (chunk) {
                    this->m_mapChunks[16 * chunk->m_aIndex.y + chunk->m_aIndex.x] = nullptr;
                    CMap::FreeBaseObjLink(chunk->m_parentLinkList.Head());
                    this->PurgeChunk(chunk);
                }
            }
        }
    }
}

// VERIFY: 1e0fb652 manual
// OFFSET: 0x007D6D20
void CMapArea::LoadTextures(char* fileNames, uint32_t fileNamesSize) {
    uint32_t offset = 0;
    int32_t streaming = SFile::IsStreamingMode();

    this->m_textures.m_chunk = 16;

    if (!fileNamesSize) {
        return;
    }

    do {
        CMapAreaTexture* areaTexture = this->m_textures.New();
        areaTexture->m_textureName = &fileNames[offset];
        areaTexture->m_texture = nullptr;

        if (!streaming) {
            this->LoadTerrainTexture(&this->m_textures.m_data[this->m_textures.Count() - 1], this->m_textures.Count() - 1);
        }

        while (fileNames[offset]) {
            offset++;
        }

        offset++;
    } while (offset < fileNamesSize);
}

// VERIFY: 2e3ea90a manual
// OFFSET: 0x007D6EF0
void CMapArea::ProcessIffChunks() {
    SIffChunk* version = static_cast<SIffChunk*>(this->m_fileBuffer);
    uint8_t* header = reinterpret_cast<uint8_t*>(&version->m_data) + version->m_size + offsetof(SIffChunk, m_data);

    this->m_header = reinterpret_cast<SMAreaHeader*>(header);
    this->m_chunkInfo = reinterpret_cast<SMChunkInfo*>(&reinterpret_cast<SIffChunk*>(header + this->m_header->m_mcin)->m_data);
    char* textureNames = reinterpret_cast<char*>(&reinterpret_cast<SIffChunk*>(header + this->m_header->m_mtex)->m_data);
    this->m_doodadDef = reinterpret_cast<SMDoodadDef*>(&reinterpret_cast<SIffChunk*>(header + this->m_header->m_mddf)->m_data);
    this->m_mapObjDef = reinterpret_cast<SMMapObjDef*>(&reinterpret_cast<SIffChunk*>(header + this->m_header->m_modf)->m_data);
    this->m_m2FileNames = reinterpret_cast<char*>(&reinterpret_cast<SIffChunk*>(header + this->m_header->m_mmdx)->m_data);
    this->m_wmoFileNames = reinterpret_cast<char*>(&reinterpret_cast<SIffChunk*>(header + this->m_header->m_mwmo)->m_data);
    this->m_modelFilenamesOffsets = reinterpret_cast<uint32_t*>(&reinterpret_cast<SIffChunk*>(header + this->m_header->m_mmid)->m_data);
    this->m_wmoFilenamesOffsets = reinterpret_cast<uint32_t*>(&reinterpret_cast<SIffChunk*>(header + this->m_header->m_mwid)->m_data);

    if (this->m_header->m_flags & 0x1) {
        this->m_flyingBbox = reinterpret_cast<int16_t*>(&reinterpret_cast<SIffChunk*>(header + this->m_header->m_mfbo)->m_data);
    }

    if (this->m_header->m_mh2o && reinterpret_cast<SIffChunk*>(header + this->m_header->m_mh2o)->m_size) {
        void* mem = SMemAlloc(sizeof(Liquid::CAreaData), __FILE__, __LINE__, 0);
        Liquid::CAreaData* liquid = mem ? new (mem) Liquid::CAreaData(nullptr) : nullptr;
        this->m_liquid = liquid;
        liquid->SetData(&reinterpret_cast<SIffChunk*>(header + this->m_header->m_mh2o)->m_data);
    }

    if (this->m_header->m_mtxf) {
        this->m_textureFlags = reinterpret_cast<int32_t*>(&reinterpret_cast<SIffChunk*>(header + this->m_header->m_mtxf)->m_data);
    }

    this->LoadTextures(textureNames, reinterpret_cast<SIffChunk*>(header + this->m_header->m_mtex)->m_size);

    this->m_doodadDefCount = reinterpret_cast<SIffChunk*>(header + this->m_header->m_mddf)->m_size / sizeof(SMDoodadDef);
    this->m_mapObjDefCount = reinterpret_cast<SIffChunk*>(header + this->m_header->m_modf)->m_size / sizeof(SMMapObjDef);
}

// VERIFY: ff270d7a auto
// OFFSET: 0x007D7020
void CMapArea::AsyncCallback(void* param) {
    CMapArea* area = static_cast<CMapArea*>(param);

    area->ProcessIffChunks();
    AsyncFileReadDestroyObject(area->m_asyncObject);
    area->m_asyncObject = nullptr;
    area->m_file = nullptr;
}

// OFFSET: 0x007D7050
CMapArea::CMapArea() {
    this->m_index.x = 0;
    this->m_index.y = 0;
    this->m_tileChunkIndex.x = 0;
    this->m_tileChunkIndex.y = 0;
    this->m_chunkLinkList.SetLinkOffset(offsetof(CMapBaseObjLink, m_refLink));
    this->m_file = nullptr;
    this->m_asyncObject = nullptr;
    memset(this->m_mapChunks, 0, sizeof(this->m_mapChunks));
    this->m_fileBuffer = nullptr;
    this->m_fileSize = 0;
    this->unk_008C = 0;
    this->m_doodadDef = nullptr;
    this->m_mapObjDef = nullptr;
    this->m_chunkInfo = nullptr;
    this->m_doodadDefCount = 0;
    this->m_mapObjDefCount = 0;
    this->m_m2FileNames = nullptr;
    this->m_wmoFileNames = nullptr;
    this->m_modelFilenamesOffsets = nullptr;
    this->m_wmoFilenamesOffsets = nullptr;
    this->m_flyingBbox = nullptr;
    this->m_textureFlags = nullptr;
    this->m_liquid = nullptr;
}

// VERIFY: 09d8cf56 auto
// OFFSET: 0x007D7150
void CMapArea::Load(const char* fileName) {
    CMap::SafeOpen(fileName, &this->m_file);
    this->m_fileSize = SFileGetFileSize(this->m_file, nullptr);
    this->m_fileBuffer = CMapArea::AllocAsyncLoadBuffer(this->m_fileSize);
    this->m_asyncObject = AsyncFileReadAllocObject();
    this->m_asyncObject->m_file = this->m_file;
    this->m_asyncObject->m_buffer = this->m_fileBuffer;
    this->m_asyncObject->m_size = this->m_fileSize;
    this->m_asyncObject->m_userArg = this;
    this->m_asyncObject->m_userPostloadCallback = reinterpret_cast<void*>(&CMapArea::AsyncCallback);
    AsyncFileReadObject(this->m_asyncObject, 0);
}
