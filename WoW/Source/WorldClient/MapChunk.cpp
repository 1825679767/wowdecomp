#include "MapChunk.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/WorldClient/CChunkLiquid.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/DB/Rec/LiquidTypeRec.h"
#include "WoW/Source/DB/Rec/LiquidMaterialRec.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"
#include <cmath>
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "Engine/Source/Tempest/C2iVector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CFacet.h"
#include "Engine/Source/Tempest/CFrustum.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/WorldClient/CMapBaseObjLink.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Base/RCString.h"
#include "WoW/Source/WorldClient/Types.h"
#include "Engine/Source/Gxu/GxuLight.h"

// STATIC: 0x00D25488
float CMapChunk::s_geoToTex;

// STATIC: 0x00D25498
C3Vector CMapChunk::vertexList[145];

// STATIC: 0x00AEEE3C
const int32_t CMapChunk::cornerVertexIndex[4] = { 0, 8, 136, 144 };

// STATIC: 0x00CD8F38
int32_t CMapChunk::farCornerIndex;

// OFFSET: 0x007C3A10
void CMapChunk::ProcessIffChunks(bool a3) {
    SIffChunk* headerChunk = reinterpret_cast<SIffChunk*>(this->m_chunkInfoBeginPtr);
    this->m_header = reinterpret_cast<SMChunk*>(&headerChunk->m_data);

    int32_t remaining = headerChunk->m_size - sizeof(SMChunk);
    SIffChunk* chunk = reinterpret_cast<SIffChunk*>(reinterpret_cast<uint8_t*>(this->m_header) + sizeof(SMChunk));

    while (remaining > 0) {
        uint8_t* data = reinterpret_cast<uint8_t*>(&chunk->m_data);
        remaining -= offsetof(SIffChunk, m_data);

        switch (chunk->m_token) {
            case 0x4D434E52:
                this->m_normals = reinterpret_cast<int8_t*>(data);

                if (a3) {
                    chunk->m_size = 448;
                }

                break;

            case 0x4D435348:
                this->m_shadowMap = data;
                break;

            case 0x4D435654:
                this->m_height = reinterpret_cast<float*>(data);
                break;

            case 0x4D435246:
                this->m_mcrfPtr = data;
                break;

            case 0x4D435345:
                if (this->m_header->m_nSndEmitters) {
                    this->m_soundEmitters = reinterpret_cast<CWSoundEmitter*>(data);
                }

                break;

            case 0x4D434C59:
                this->m_layers = reinterpret_cast<SMLayer*>(data);
                break;

            case 0x4D434C51:
                if (this->m_header->m_sizeLiquid > 8) {
                    this->m_liquid = reinterpret_cast<SMLiquidChunk*>(data);
                    chunk->m_size = this->m_header->m_sizeLiquid - 8;
                }

                break;

            case 0x4D43414C:
                this->m_additionalShadowmap = data;

                if (a3) {
                    chunk->m_size = this->m_header->m_sizeAlpha - 8;
                }

                break;

            case 0x4D434356:
                this->m_vertexShading = reinterpret_cast<uint32_t*>(data);
                break;
        }

        remaining -= chunk->m_size;
        chunk = reinterpret_cast<SIffChunk*>(data + chunk->m_size);
    }
}

// VERIFY: 86d6cd8c manual
// OFFSET: 0x007C3C60
void CMapChunk::InitializeVertexGrid() {
    C3Vector* vertex = CMapChunk::vertexList;

    for (int32_t row = 0; row < 9; row++) {
        float x = row * -4.1666665f;

        for (int32_t col = 0; col < 9; col++) {
            vertex->x = x;
            vertex->y = 0.0f - col * 4.1666665f;
            vertex++;
        }

        if (row < 8) {
            for (int32_t col = 0; col < 8; col++) {
                vertex->x = x - 2.0833333f;
                vertex->y = 0.0f - col * 4.1666665f - 2.0833333f;
                vertex++;
            }
        }
    }
}

// VERIFY: 6ef0b88b auto
// OFFSET: 0x007C3D90
void CMapChunk::Initialize() {
    CMapRenderChunk::Initialize();
    CMapChunk::InitializeVertexGrid();
    CMapChunk::s_geoToTex = -1.0f / CMapChunk::vertexList[1].y;
}

// VERIFY: b5717665 auto
// OFFSET: 0x007C3A00
void CMapChunk::Destroy() {
    CMapRenderChunk::Destroy();
}


// VERIFY: 5994fc04 auto
// OFFSET: 0x007C3B40
void CMapChunk::AllocRenderChunkAndBatch() {
    this->m_renderChunk = CMap::AllocRenderChunk();
    this->m_renderChunk->AddBatch(this, nullptr, &this->m_topLeftCoords, 0);
}

// OFFSET: 0x007C3B60
int16_t CMapChunk::CreateIndices(char* buf, int32_t baseVertex) {
    uint16_t* indexDst = reinterpret_cast<uint16_t*>(buf);
    int16_t totalIndices = 0;

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            // Holes are stored as a 4×4 bitmask (one bit per 2×2 block of cells).
            int holeMaskIndex = (row / 2) * 4 + (col / 2);
            if (this->m_header->m_holes & CMap::s_holeMask[holeMaskIndex])
                continue;

            // The five vertices that define this cell.
            // Each outer row has 9 verts, each inner row has 8 — so 17 per row-pair.
            uint16_t TL = baseVertex + col;
            uint16_t TR = baseVertex + col + 1;
            uint16_t C = baseVertex + col + 9;   // inner vertex, midpoint of cell
            uint16_t BL = baseVertex + col + 17; // next outer row
            uint16_t BR = baseVertex + col + 18;

            // Four triangles, all fanning from the center point (CCW winding).
            indexDst[0] = C;
            indexDst[1] = TL;
            indexDst[2] = BL; // left
            indexDst[3] = C;
            indexDst[4] = TR;
            indexDst[5] = TL; // top
            indexDst[6] = C;
            indexDst[7] = BR;
            indexDst[8] = TR; // right
            indexDst[9] = C;
            indexDst[10] = BL;
            indexDst[11] = BR; // bottom
            indexDst += 12;
            totalIndices += 12;
        }

        // Advance past this row-pair (9 outer + 8 inner vertices).
        baseVertex += 17;
    }

    return totalIndices;
}

// OFFSET: 0x007C4960
void CMapChunk::CreateVerticesLocal(CGxVertexPN* v) {
    // ~1/127: scales packed signed-byte normals into [-1, 1] float range
    static const float NORMAL_SCALE = 0.0078740157f;

    // Both expressions simplify to: 33.333332f * 0.125f = ~4.1667 units/step
    const float stepX = (17066.666f - (float)this->m_cOffset.x * 33.333332f - (17066.666f - (float)(this->m_cOffset.x + 1) * 33.333332f)) * -0.125f;
    const float stepY = (17066.666f - (float)this->m_cOffset.y * 33.333332f - (17066.666f - (float)(this->m_cOffset.y + 1) * 33.333332f)) * -0.125f;

    const float halfStepX = stepX * 0.5f;

    float* height = this->m_height;
    int8_t* normals = this->m_normals;

    for (int row = 0; row < 9; row++) {
        // --- Outer grid row: 9 evenly-spaced vertices ---
        const float outerX = (float)row * stepY;

        for (int col = 0; col < 9; col++) {
            v->m_position.x = outerX;
            v->m_position.y = (float)col * stepX;
            v->m_position.z = height[col];
            v->m_normal.x = normals[col * 3 + 0] * NORMAL_SCALE;
            v->m_normal.y = normals[col * 3 + 1] * NORMAL_SCALE;
            v->m_normal.z = normals[col * 3 + 2] * NORMAL_SCALE;
            v++;
        }

        height += 9;
        normals += 27; // 9 vertices * 3 components

        // --- Inner (center-point) grid row: 8 vertices, staggered by half a step ---
        // The last outer row (row == 8) has no corresponding inner row
        if (row < 8) {
            const float innerX = (float)row * stepX + 0.5f * stepY;

            for (int col = 0; col < 8; col++) {
                v->m_position.x = innerX;
                v->m_position.y = (float)col * stepY + halfStepX;
                v->m_position.z = height[col];
                v->m_normal.x = normals[col * 3 + 0] * NORMAL_SCALE;
                v->m_normal.y = normals[col * 3 + 1] * NORMAL_SCALE;
                v->m_normal.z = normals[col * 3 + 2] * NORMAL_SCALE;
                v++;
            }

            height += 8;
            normals += 24; // 8 vertices * 3 components
        }
    }
}


// OFFSET: 0x007C4F10
void CMapChunk::CreateVerticesLocal(CGxVertexPNC* v) {
    float stepX = (17066.666f - this->m_cOffset.x * 33.333332f - (17066.666f - (this->m_cOffset.x + 1) * 33.333332f)) * -0.125f;
    float stepY = -0.125f * (17066.666f - this->m_cOffset.y * 33.333332f - (17066.666f - 33.333332f * (this->m_cOffset.y + 1)));
    float halfStepX = stepX * 0.5f;
    float halfStepY = 0.5f * stepY;

    float* height = this->m_height;
    int8_t* normals = this->m_normals;
    uint32_t* shading = this->m_vertexShading;

    for (int32_t row = 0; row < 9; row++) {
        for (int32_t col = 0; col < 9; col++) {
            v->m_position.x = stepY * row;
            v->m_position.y = col * stepX;
            v->m_position.z = *height;
            v->m_normal.x = normals[0] * 0.0078740157f;
            v->m_normal.y = normals[1] * 0.0078740157f;
            v->m_normal.z = normals[2] * 0.0078740157f;

            if (this->m_vertexShading) {
                CImVector color = *reinterpret_cast<CImVector*>(shading);

                if (g_theGxDevicePtr->Caps().m_colorFormat == GxCF_rgba) {
                    CImVector swapped;
                    swapped.b = color.r;
                    swapped.g = color.g;
                    swapped.r = color.b;
                    swapped.a = color.a;
                    v->m_color = swapped;
                } else {
                    v->m_color = color;
                }
            } else {
                *reinterpret_cast<uint32_t*>(&v->m_color) = 0xFF7F7F7F;
            }

            height++;
            v++;
            normals += 3;
            shading++;
        }

        if (row < 8) {
            float innerX = row * stepX + halfStepY;

            for (int32_t col = 0; col < 8; col++) {
                v->m_position.x = innerX;
                v->m_position.y = col * stepY + halfStepX;
                v->m_position.z = *height;
                v->m_normal.x = normals[0] * 0.0078740157f;
                v->m_normal.y = normals[1] * 0.0078740157f;
                v->m_normal.z = normals[2] * 0.0078740157f;

                if (this->m_vertexShading) {
                    CImVector color = *reinterpret_cast<CImVector*>(shading);

                    if (g_theGxDevicePtr->Caps().m_colorFormat == GxCF_rgba) {
                        CImVector swapped;
                        swapped.b = color.r;
                        swapped.g = color.g;
                        swapped.r = color.b;
                        swapped.a = color.a;
                        v->m_color = swapped;
                    } else {
                        v->m_color = color;
                    }
                } else {
                    *reinterpret_cast<uint32_t*>(&v->m_color) = 0xFF7F7F7F;
                }

                height++;
                v++;
                normals += 3;
                shading++;
            }
        }
    }
}

// OFFSET: 0x007C51B0
void CMapChunk::CreateIndices(uint16_t* buf, CGxBatch* batch) {
    int32_t baseVertex = batch->m_maxIndex ? batch->m_maxIndex + 1 : 0;
    int16_t count = this->CreateIndices(reinterpret_cast<char*>(buf), baseVertex);

    uint16_t minIndex = batch->m_minIndex;

    if (minIndex >= baseVertex) {
        minIndex = baseVertex;
    }

    batch->m_minIndex = minIndex;

    int32_t maxIndex = CMapRenderChunk::unk_AEEC60.m_maxIndex + baseVertex;

    if (batch->m_maxIndex > maxIndex) {
        maxIndex = batch->m_maxIndex;
    }

    batch->m_maxIndex = maxIndex;
    batch->m_count += count;
}

// OFFSET: 0x007C5220
void CMapChunk::CreateBounds() {
    int32_t tileX = this->m_cOffset.x;
    int32_t tileY = this->m_cOffset.y;

    this->m_bbox.b.x = 17066.666f - 33.333332f * (tileY + 1);
    this->m_bbox.t.x = 17066.666f - 33.333332f * tileY;
    this->m_bbox.b.y = 17066.666f - 33.333332f * (tileX + 1);
    this->m_bbox.t.y = 17066.666f - 33.333332f * tileX;
    this->m_bbox.b.z = 3.4028235e38f;
    this->m_bbox.t.z = -3.4028235e38f;

    float* h = this->m_height;
    int count = 29;
    do {
        for (int i = 0; i < 5; i++) {
            float height = h[i];
            if (this->m_bbox.b.z > height)
                this->m_bbox.b.z = height;
            if (this->m_bbox.t.z < height)
                this->m_bbox.t.z = height;
        }
        h += 5;
    } while (--count);

    this->m_bbox.b.z += this->m_topLeftCoords.z;
    this->m_bbox.t.z += this->m_topLeftCoords.z;

    this->m_sphere.c.x = (this->m_bbox.b.x + this->m_bbox.t.x) * 0.5f;
    this->m_sphere.c.y = (this->m_bbox.b.y + this->m_bbox.t.y) * 0.5f;
    this->m_sphere.c.z = (this->m_bbox.b.z + this->m_bbox.t.z) * 0.5f;

    this->m_bbox2 = this->m_bbox;

    float ex = this->m_bbox.t.x - this->m_sphere.c.x;
    float ey = this->m_bbox.t.y - this->m_sphere.c.y;
    float ez = this->m_bbox.t.z - this->m_sphere.c.z;
    this->m_sphere.r = sqrt(ex * ex + ey * ey + ez * ez);
}

// VERIFY: baa581e9 auto
// OFFSET: 0x007C5440
void CMapChunk::Batch() {
    if (this->m_bLoaded) {
        return;
    }

    if (CMap::enableChunkBatching) {
        CMapArea* area = static_cast<CMapArea*>(this->m_parentLinkList.Head()->m_ref);

        C2iVector pos;
        pos.x = this->m_aIndex.x & ~1;
        pos.y = this->m_aIndex.y & ~1;
        area->BatchChunks(pos);
    } else {
        this->m_renderChunk = CMap::AllocRenderChunk();
        this->m_renderChunk->AddBatch(this, nullptr, &this->m_topLeftCoords, 0);
        this->m_bLoaded = 1;
    }
}

// VERIFY: 2c8f1c5e auto
// OFFSET: 0x007C54C0
void CMapChunk::CreateVertices(char* buf, int32_t vertexOffset, const C3Vector& offset) {
    if (CMap::enableTerrainShaderVertex) {
        if (CMapRenderChunk::s_gxBufVertexFormat == GxVBF_PN) {
            this->CreateVerticesWorld(reinterpret_cast<CGxVertexPN*>(buf) + vertexOffset, offset);
        } else {
            this->CreateVerticesWorld(reinterpret_cast<CGxVertexPNC*>(buf) + vertexOffset, offset);
        }
    } else if (CMapRenderChunk::s_gxBufVertexFormat == GxVBF_PN) {
        this->CreateVerticesLocal(reinterpret_cast<CGxVertexPN*>(buf));
    } else {
        this->CreateVerticesLocal(reinterpret_cast<CGxVertexPNC*>(buf));
    }
}

// VERIFY: 4f0133d9 auto
// OFFSET: 0x007C5530
void CMapChunk::GetLiquidBounds(CAaBox& box) const {
    box.t = this->m_bbox.t;
    box.b = this->m_bbox.b;

    CChunkLiquid* liquid = const_cast<CMapChunk*>(this)->m_liquidChunkLinkList.Head();

    if (liquid) {
        box.t.z = liquid->m_height.y;
        box.b.z = liquid->m_height.x;

        do {
            box.t.z = liquid->m_height.y >= box.t.z ? liquid->m_height.y : box.t.z;
            box.b.z = liquid->m_height.x <= box.b.z ? liquid->m_height.x : box.b.z;
            liquid = const_cast<CMapChunk*>(this)->m_liquidChunkLinkList.Next(liquid);
        } while (liquid);
    }
}

// VERIFY: 16a231ad manual
// OFFSET: 0x007C55D0
bool CMapChunk::GetLiquidHeight(const C2Vector& pos, float* height) const {
    int32_t x = CMath::fint_(pos.x - 0.5f);
    int32_t y = CMath::fint_(pos.y - 0.5f);

    C2iVector tile;
    tile.x = x & 7;
    tile.y = y & 7;

    C2Vector frac(pos.x - x, pos.y - y);

    for (CChunkLiquid* liquid = const_cast<CMapChunk*>(this)->m_liquidChunkLinkList.Head(); liquid; liquid = const_cast<CMapChunk*>(this)->m_liquidChunkLinkList.Next(liquid)) {
        if (liquid->TileExists(tile.x, tile.y) && liquid->GetHeight(frac, tile, *height)) {
            return true;
        }
    }

    return false;
}

// VERIFY: 981b86cf manual
// OFFSET: 0x007C5690
void CMapChunk::CreateLiquids(bool a3) {
    CChunkLiquid* liquid = this->m_liquidChunkLinkList.Head();

    while (liquid) {
        liquid->m_chunkLink.Unlink();
        liquid->Purge();
        CMap::FreeChunkLiquid(liquid);
        liquid = this->m_liquidChunkLinkList.Next(liquid);
    }

    SMLiquidChunk* mclq = this->m_liquid;

    if (mclq) {
        uint32_t flag = 4;

        for (uint32_t i = 0; i < 4; i++) {
            if (flag & this->m_header->m_flags) {
                uint32_t type = i + 1;

                if (s_mapId == 530 && i == 2) {
                    type = 15;
                }

                LiquidTypeRec* typeRec = g_liquidTypeDB.GetRecord(type);
                LiquidMaterialRec* materialRec = g_liquidMaterialDB.GetRecord(typeRec->m_materialID);

                CChunkLiquid* chunkLiquid = CMap::AllocChunkLiquid();
                this->m_liquidChunkLinkList.LinkToTail(chunkLiquid);
                chunkLiquid->m_type = type;
                chunkLiquid->m_owner = this;
                chunkLiquid->unk_0008 = materialRec->m_lvf;
                chunkLiquid->m_topLeftCoords = this->m_topLeftCoords;
                chunkLiquid->m_height = *reinterpret_cast<C2Vector*>(&mclq->m_minHeight);

                uint8_t* verts = reinterpret_cast<uint8_t*>(mclq->m_verts);
                chunkLiquid->m_tileBegin.x = 0;
                chunkLiquid->m_tileBegin.y = 0;
                chunkLiquid->m_tileEnd.x = 8;
                chunkLiquid->m_tileEnd.y = 8;

                void* mem = SMemAlloc(sizeof(Liquid::CChunkVertexData), __FILE__, __LINE__, 0);
                Liquid::CChunkVertexData* vertexData = mem ? new (mem) Liquid::CChunkVertexData(verts, chunkLiquid->unk_0008) : nullptr;
                chunkLiquid->m_tiles = reinterpret_cast<uint8_t*>(&mclq->m_tiles);
                chunkLiquid->m_vertexData = vertexData;
                mclq++;

                chunkLiquid->CreateVertXY();
            }

            flag *= 2;
        }
    }

    CMapArea* area = static_cast<CMapArea*>(this->m_parentLinkList.Head()->m_ref);

    if (area->m_liquid && area->m_liquid->HasData()) {
        const SMLiquidChunkDesc* chunkDesc = area->m_liquid->GetChunkDesc(this->m_aIndex.x, this->m_aIndex.y);
        this->m_deepBits.SetRawData(const_cast<char*>(reinterpret_cast<const char*>(area->m_liquid->GetDeepTable(chunkDesc))), 64, false);
        this->m_fishableBits.SetRawData(const_cast<char*>(reinterpret_cast<const char*>(area->m_liquid->GetFishableTable(chunkDesc))), 64, false);

        for (uint32_t i = 0; i < chunkDesc->m_layerCount; i++) {
            const SMLiquidInstance* instance = area->m_liquid->GetInstanceDesc(chunkDesc, i);

            CChunkLiquid* chunkLiquid = CMap::AllocChunkLiquid();
            this->m_liquidChunkLinkList.LinkToTail(chunkLiquid);
            chunkLiquid->m_owner = this;
            chunkLiquid->m_type = instance->m_liquidType;
            chunkLiquid->unk_0008 = instance->m_vertexFormat;
            chunkLiquid->m_topLeftCoords = this->m_topLeftCoords;
            chunkLiquid->m_height = *reinterpret_cast<const C2Vector*>(&instance->m_minHeight);

            uint8_t yOffset = instance->m_yOffset;
            uint8_t xOffset = instance->m_xOffset;
            chunkLiquid->m_tileBegin.x = yOffset;
            chunkLiquid->m_tileBegin.y = xOffset;
            chunkLiquid->m_tileEnd.x = yOffset + instance->m_height;
            chunkLiquid->m_tileEnd.y = xOffset + instance->m_width;

            void* mem = SMemAlloc(sizeof(Liquid::CInstanceVertexData), __FILE__, __LINE__, 0);
            chunkLiquid->m_vertexData = mem ? new (mem) Liquid::CInstanceVertexData(area->m_liquid, instance) : nullptr;

            uint32_t tileCount = instance->m_width * instance->m_height;
            chunkLiquid->m_tileBits.SetRawData(const_cast<char*>(reinterpret_cast<const char*>(area->m_liquid->GetExistsTable(instance))), tileCount, false);
            chunkLiquid->CreateVertXY();
        }
    }

    if (!this->m_liquidChunkLinkList.Head()) {
        this->m_liquidBounds.b = C3Vector(3.4028235e38f, 3.4028235e38f, 3.4028235e38f);
        this->m_liquidBounds.t = C3Vector(-3.4028235e38f, -3.4028235e38f, -3.4028235e38f);
    } else {
        this->GetLiquidBounds(this->m_liquidBounds);
    }
}

// VERIFY: 2c9fa8ff manual
// OFFSET: 0x007C5B20
void CMapChunk::CullAndBucketChunk() {
    if (this->m_liquidBounds.t.x < this->m_liquidBounds.b.x || CWorldScene::FrustumCull(&this->m_liquidBounds)) {
        return;
    }

    int32_t index = CMapChunk::cornerVertexIndex[CMapChunk::farCornerIndex];
    float camZ = CWorldScene::s_activeWorldView.z;
    C3Vector corner(CMapChunk::vertexList[index].x + this->m_topLeftCoords.x, CMapChunk::vertexList[index].y + this->m_topLeftCoords.y, this->m_height[index] + this->m_topLeftCoords.z);

    for (CChunkLiquid* liquid = this->m_liquidChunkLinkList.Head(); liquid; liquid = this->m_liquidChunkLinkList.Next(liquid)) {
        CAaBox box;
        liquid->GetAaBox(box);

        if (!CWorldScene::FrustumCull(&box)) {
            corner.z = (camZ <= liquid->m_height.y ? camZ : liquid->m_height.y) < liquid->m_height.x ? liquid->m_height.x : (camZ > liquid->m_height.y ? liquid->m_height.y : camZ);
            CWorldScene::AddToDistanceBucket(liquid, corner);
        }
    }
}

// OFFSET: 0x007C5C50
CMapChunk::CMapChunk() {
    this->m_aIndex.x = 0;
    this->m_aIndex.y = 0;
    this->m_sOffset.x = 0;
    this->m_sOffset.y = 0;
    this->m_cOffset.x = 0;
    this->m_cOffset.y = 0;
    this->m_sphere.r = 0.0f;
    this->m_sphere.c = C3Vector(0.0f, 0.0f, 0.0f);
    this->unk_00B4 = 0;
    this->unk_00B8 = 0;
    this->m_sortListLink.m_prevLink = nullptr;
    this->m_sortListLink.m_next = nullptr;
    this->m_doodadDefLinkList.SetLinkOffset(offsetof(CMapBaseObjLink, m_refLink));
    this->m_mapObjDefLinkList.SetLinkOffset(offsetof(CMapBaseObjLink, m_refLink));
    this->m_entityLinkList.SetLinkOffset(offsetof(CMapBaseObjLink, m_refLink));
    this->m_lightLinkList.SetLinkOffset(offsetof(CMapBaseObjLink, m_refLink));
    this->m_soundHandleList.SetLinkOffset(0);
    this->m_type |= 4;
    this->m_distToCamera = 0.0f;
    this->m_flags = static_cast<MapObjFlag>(this->m_flags | MAPOBJ_FLAG_UNPLACED);
    this->m_detailDoodadInst = nullptr;
    this->m_renderChunk = nullptr;
    this->m_chunkInfoBeginPtr = nullptr;
    this->m_header = nullptr;
    this->m_lowQualityTexMap = nullptr;
    this->m_predTexture = nullptr;
    this->m_height = nullptr;
    this->m_vertexShading = nullptr;
    this->m_normals = nullptr;
    this->m_shadowMap = nullptr;
    this->m_layers = nullptr;
    this->m_additionalShadowmap = nullptr;
    this->m_mcrfPtr = nullptr;
    this->m_liquid = nullptr;
    this->m_soundEmitters = nullptr;
    this->m_bLoaded = 0;
}

// OFFSET: 0x007C64B0
void CMapChunk::Create(SIffChunk* headerChunk, bool a3) {
    this->m_chunkInfoBeginPtr = reinterpret_cast<uint8_t*>(headerChunk);
    this->ProcessIffChunks(a3);

    this->m_areaId = this->m_header->m_areaid;
    this->m_topLeftCoords.x = 17066.666f - this->m_cOffset.y * 33.333332f;
    this->m_topLeftCoords.y = 17066.666f - this->m_cOffset.x * 33.333332f;
    this->m_topLeftCoords.z = 0.0f;
    this->m_topLeftCoords.z = this->m_header->m_position.z;
    this->m_lowQualityTexMap = this->m_header->m_lowQualityTextureMap;
    this->m_predTexture = reinterpret_cast<uint8_t*>(&this->m_header->m_predTex);

    this->CreateBounds();
    this->CreateLiquids(a3);
    this->CreateSoundEmitters(a3);

    this->m_flags = static_cast<MapObjFlag>(0);

    if (this->m_header->m_flags & 2) {
        this->m_flags = MAPOBJ_FLAG_IMPASSABLE;
    }

    CMapArea* area = static_cast<CMapArea*>(this->m_parentLinkList.Head()->m_ref);
    this->CreateRefs(area, reinterpret_cast<uint32_t*>(this->m_mcrfPtr), this->m_header->m_nDoodadRefs, this->m_header->m_nMapObjRefs);
    area->m_mapChunks[16 * this->m_aIndex.y + this->m_aIndex.x] = this;
    this->m_flags = static_cast<MapObjFlag>(this->m_flags | MAPOBJ_FLAG_PREPARED);
}

// VERIFY: 1d9777de manual
// OFFSET: 0x007C65A0
void CMapChunk::UpdateLights() {
    for (CMapBaseObjLink* link = this->m_doodadDefLinkList.Head(); link; link = this->m_doodadDefLinkList.Next(link)) {
        link->m_owner->m_flags = static_cast<MapObjFlag>(link->m_owner->m_flags | 0x1);
    }

    for (CMapBaseObjLink* link = this->m_entityLinkList.Head(); link; link = this->m_entityLinkList.Next(link)) {
        link->m_owner->m_flags = static_cast<MapObjFlag>(link->m_owner->m_flags | 0x1);
    }
}

// VERIFY: 967e03e0 auto
// OFFSET: 0x007D3FE0
void CMapChunk::RenderPrep() {
    if (this->m_renderChunk || (this->Batch(), this->m_renderChunk)) {
        float dy = CWorldScene::s_activeWorldView.y - this->m_sphere.c.y;
        float dz = CWorldScene::s_activeWorldView.z - this->m_sphere.c.z;
        float dx = CWorldScene::s_activeWorldView.x - this->m_sphere.c.x;

        if (CWorld::shadowMipLevel || dx * dx + dz * dz + dy * dy > 603729.0f) {
            this->m_renderChunk->unk_000A |= 0x10;
        }

        this->m_renderChunk->RenderPrep();
    }

    if ((CWorld::enables & Enable_DetailDoodads) && CWorld::detailDoodadDist > this->m_distToCamera) {
        if (!this->m_detailDoodadInst) {
            this->CreateDetailDoodads();
        }

        if (this->m_detailDoodadInst) {
            CWorldScene::AddChunkToSortList(static_cast<CDetailDoodadInst*>(this->m_detailDoodadInst));
        }
    }
}

// OFFSET: 0x007D66D0
bool CMapChunk::CanMergeChunkLayers(CMapChunk* chunkA, CMapChunk* chunkB) {
    chunkB->m_bLoaded = 1;
    chunkA->m_bLoaded = 1;

    if (CMap::header.m_flags & 4) {
        uint32_t count = chunkB->m_header->m_nLayers;
        SMLayer* layerA = chunkA->m_layers;

        for (uint32_t i = 0; i < chunkA->m_header->m_nLayers; i++, layerA++) {
            if (layerA->m_flags & 0x4C0) {
                return false;
            }

            SMLayer* layerB = chunkB->m_layers;
            bool found = false;

            for (uint32_t j = 0; j < chunkB->m_header->m_nLayers; j++, layerB++) {
                if (layerB->m_flags & 0x4C0) {
                    return false;
                }

                if (layerB->m_textureId == layerA->m_textureId) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                count++;
            }
        }

        return count <= 4;
    }

    uint32_t layerCount = chunkA->m_header->m_nLayers;

    if (chunkB->m_header->m_nLayers != layerCount) {
        return false;
    }

    for (uint32_t i = 0; i < layerCount; i++) {
        SMLayer& layerA = chunkA->m_layers[i];
        SMLayer& layerB = chunkB->m_layers[i];

        if ((layerA.m_flags & 0xC0) || (layerB.m_flags & 0xC0) || layerB.m_textureId != layerA.m_textureId) {
            return false;
        }
    }

    return true;
}
