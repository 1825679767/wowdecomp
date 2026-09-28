#include "MapObjGroup.h"
#include <cmath>
#include <malloc.h>
#include "Engine/Source/Tempest/C2iVector.h"
#include "Engine/Source/Tempest/C3Ray.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CWFrustum.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "Engine/Source/Gx/Gx.h"

// STATIC: 0x00AEEE58
VBBList CMapObjGroup::vertexVBList;

// STATIC: 0x00AEEE80
VBBList CMapObjGroup::indexVBList;

// STATIC: 0x00AEEEA8
TSExplicitList<CMapObjGroup::VertArray, offsetof(CMapObjGroup::VertArray, m_link)> CMapObjGroup::s_unk_AEEEA8;

// VERIFY: 4f76965b auto
// OFFSET: 0x007C7790
static uint8_t CullLiquidVertexZ(const CAaBox& box, const C3Vector& vertex) {
    float below = vertex.z - box.b.z + 0.019444443f;
    float above = box.t.z - vertex.z + 0.019444443f;
    return ((*reinterpret_cast<uint32_t*>(&below) >> 29) & 0x4) | ((*reinterpret_cast<uint32_t*>(&above) >> 26) & 0x20);
}

// VERIFY: 03d37c83 auto
// OFFSET: 0x007CB990
void CMapObjGroup::Initialize() {
    CMapObjGroup::vertexVBList.m_singlePool = 0;
    CMapObjGroup::vertexVBList.m_target = GxPoolTarget_Vertex;
    CMapObjGroup::vertexVBList.m_usage = static_cast<EGxPoolUsage>(CWorld::unk_CD7658);
    CMapObjGroup::vertexVBList.m_pool = nullptr;
    CMapObjGroup::indexVBList.m_singlePool = 0;
    CMapObjGroup::indexVBList.m_target = GxPoolTarget_Index;
    CMapObjGroup::indexVBList.m_usage = static_cast<EGxPoolUsage>(CWorld::unk_CD7658);
    CMapObjGroup::indexVBList.m_pool = nullptr;
}

// VERIFY: 6dcd01f3 manual
// OFFSET: 0x007CB870
void VBBList::Destroy() {
    VBBList_Block* block = this->unk_001C.Head();

    while (!this->unk_001C.IsEnd(block)) {
        VBBList_Block* next = this->unk_001C.RawNext(block);
        CGxBuf* buffer = block->m_buffer;

        if (buffer && buffer != g_theGxDevicePtr->BufStream(buffer->m_pool->m_target, 0, 0)) {
            g_theGxDevicePtr->BufDestroy(buffer);
        }

        block->m_buffer = nullptr;

        if (!this->m_singlePool) {
            if (block->m_pool) {
                g_theGxDevicePtr->PoolDestroy(block->m_pool);
            }

            block->m_pool = nullptr;
        }

        this->unk_001C.DeleteNode(block);
        block = next;
    }

    if (this->m_singlePool) {
        block = this->unk_0010.Head();

        while (!this->unk_0010.IsEnd(block)) {
            VBBList_Block* next = this->unk_0010.RawNext(block);
            CGxBuf* buffer = block->m_buffer;

            if (buffer && buffer != g_theGxDevicePtr->BufStream(buffer->m_pool->m_target, 0, 0)) {
                g_theGxDevicePtr->BufDestroy(buffer);
            }

            block->m_buffer = nullptr;
            this->unk_0010.DeleteNode(block);
            block = next;
        }

        if (this->m_singlePool && this->m_pool) {
            g_theGxDevicePtr->PoolDestroy(this->m_pool);
        }
    }
}

// VERIFY: 431d1d34 auto
// OFFSET: 0x007CB9D0
void CMapObjGroup::Destroy() {
    CMapObjGroup::vertexVBList.Destroy();
    CMapObjGroup::indexVBList.Destroy();
    CMapObjGroup::s_unk_AEEEA8.Clear();
}




















// VERIFY: af527478 manual
// OFFSET: 0x007C8360
bool CMapObjGroup::QueryLiquid(const C3Vector& pos, int32_t* liquidType, float* level) {
    int32_t type = this->m_liquidType;

    if (!type) {
        return false;
    }

    if (!this->m_liquidTiles.x || !this->m_liquidTiles.y) {
        *liquidType = type;
        *level = 3.4028235e38f;
        return true;
    }

    float fx = (pos.x - this->m_liquidCorner.x) * 0.24f;
    float fy = (pos.y - this->m_liquidCorner.y) * 0.24f;

    C2iVector tile;
    tile.x = static_cast<int32_t>(floor(fx));
    tile.y = static_cast<int32_t>(floor(fy));

    if (tile.x < 0 || tile.y < 0 || !(tile < this->m_liquidTiles)) {
        return false;
    }

    if ((this->m_liquidTileList[tile.y * this->m_liquidTiles.x + tile.x].m_flags & 0xF) == 0xF) {
        return false;
    }

    float dx = fx - tile.x;
    float dy = fy - tile.y;
    int32_t v0 = tile.y * this->m_liquidVerts.x + tile.x;
    int32_t v1 = this->m_liquidVerts.x + v0;
    float offset = (g_liquidTypeDB.GetRecord(type)->m_flags & 0x4) ? 0.0099999998f : 0.0f;

    const SMOLiquidVert* verts = this->m_liquidVertexList;
    float h0 = verts[v0].m_waterVert.m_height;
    float top = h0 + (verts[v0 + 1].m_waterVert.m_height - h0) * dx;
    float h2 = verts[v1].m_waterVert.m_height;
    float bottom = h2 + (verts[v1 + 1].m_waterVert.m_height - h2) * dx;
    float height = top + (bottom - top) * dy;

    if (offset + height <= pos.z) {
        return false;
    }

    *level = height;
    *liquidType = this->m_liquidType;
    return true;
}

// VERIFY: 0f477396 auto
// OFFSET: 0x007C8BF0
uint32_t CMapObjGroup::GetNumSharedTiles() {
    if (this->m_numSharedTiles > 0) {
        return this->m_numSharedTiles;
    }

    int32_t count = 0;
    const SMOLTile* tile = this->m_liquidTileList;

    for (int32_t y = 0; y < this->m_liquidTiles.y; y++) {
        for (int32_t x = 0; x < this->m_liquidTiles.x; x++) {
            if ((tile->m_flags & 0xF) != 0xF && (tile->m_flags & 0x80)) {
                count++;
            }

            tile++;
        }
    }

    this->m_numSharedTiles = count;
    return count;
}

// VERIFY: e95ab74c manual
// OFFSET: 0x007C8C60
void CMapObjGroup::GenLiquidVerts() {
    float x = this->m_liquidCorner.x;
    float y = this->m_liquidCorner.y;
    C3Vector* out = this->m_liquidVertArray->m_verts.m_data;
    const SMOLiquidVert* vert = this->m_liquidVertexList;

    this->m_liquidHeight = vert->m_waterVert.m_height;
    this->m_liquidMaxHeight = vert->m_waterVert.m_height;

    for (int32_t row = 0; row < this->m_liquidVerts.y; row++) {
        for (int32_t col = 0; col < this->m_liquidVerts.x; col++) {
            float height = vert->m_waterVert.m_height;
            out->x = x;
            out->y = y;
            out->z = height;
            out++;

            this->m_liquidHeight = height <= this->m_liquidHeight ? height : this->m_liquidHeight;
            this->m_liquidMaxHeight = height < this->m_liquidMaxHeight ? this->m_liquidMaxHeight : height;

            vert++;
            x += 4.1666665f;
        }

        x = this->m_liquidCorner.x;
        y += 4.1666665f;
    }
}

// VERIFY: 5304b11e auto
// OFFSET: 0x007CBDC0
void CMapObjGroup::AllocLiquidVB(EGxVertexBufferFormat format, uint32_t vertexCount, uint32_t indexCount, CGxBatch** batch) {
    if (!this->m_liquidVertsBlock) {
        CMapObjGroup::vertexVBList.AllocVBB(&this->m_liquidVertsBlock, GxVertexSize(format), vertexCount);
        this->m_liquidVertsBlock->m_buffer->unk_001C = 0;
    }

    if (!this->m_liquidIndicesBlock) {
        CMapObjGroup::indexVBList.AllocVBB(&this->m_liquidIndicesBlock, 2, indexCount);
        this->m_liquidIndicesBlock->m_buffer->unk_001C = 0;
    }

    *batch = &this->m_liquidBatch;
    this->m_liquidVBRefCount++;
}

// VERIFY: cd2471f5 auto
// OFFSET: 0x007CBE30
void CMapObjGroup::FreeLiquidVB() {
    if (!this->m_liquidVBRefCount) {
        return;
    }

    if (--this->m_liquidVBRefCount) {
        return;
    }

    if (this->m_liquidVertsBlock) {
        CMapObjGroup::vertexVBList.FreeVBB(this->m_liquidVertsBlock);
    }

    if (this->m_liquidIndicesBlock) {
        CMapObjGroup::indexVBList.FreeVBB(this->m_liquidIndicesBlock);
    }

    this->m_liquidVertsBlock = nullptr;
    this->m_liquidIndicesBlock = nullptr;
}

// VERIFY: 0c68f770 auto
// OFFSET: 0x007D7310
int32_t CMapObjGroup::GetLiquidType(uint32_t groupLiquid) {
    if (groupLiquid < 21 && groupLiquid) {
        switch ((groupLiquid - 1) & 3) {
            case 0:
                return (this->m_flags & 0x80000) ? 14 : 13;

            case 1:
                return 14;

            case 2:
                return 19;

            case 3:
                return 20;
        }
    }

    return groupLiquid;
}

// VERIFY: c9184804 manual
// OFFSET: 0x007C9110
void CMapObjGroup::VectorIntersectLiquidSX(const C3Vector& start, const C3Vector& end, uint32_t* tileCount, uint8_t* tiles, uint32_t maxTiles) {
    int32_t x = CMath::fint_(start.x - 0.5f);
    int32_t y = CMath::fint_(start.y - 0.5f);
    int32_t endX = CMath::fint_(end.x - 0.5f);
    int32_t step = endX >= x ? 1 : -1;
    endX += step;

    while (x != endX) {
        tiles[(*tileCount)++] = x;
        tiles[(*tileCount)++] = y;
        x += step;
    }
}

// VERIFY: 4f16d5fa manual
// OFFSET: 0x007C91A0
void CMapObjGroup::VectorIntersectLiquidSY(const C3Vector& start, const C3Vector& end, uint32_t* tileCount, uint8_t* tiles, uint32_t maxTiles) {
    int32_t x = CMath::fint_(start.x - 0.5f);
    int32_t y = CMath::fint_(start.y - 0.5f);
    int32_t endY = CMath::fint_(end.y - 0.5f);
    int32_t step = endY >= y ? 1 : -1;
    endY += step;

    while (y != endY) {
        tiles[(*tileCount)++] = x;
        tiles[(*tileCount)++] = y;
        y += step;
    }
}

// VERIFY: f3f15286 manual
// OFFSET: 0x007C9230
void CMapObjGroup::VectorIntersectLiquidDX(const C3Vector& start, const C3Vector& end, uint32_t* tileCount, uint8_t* tiles, uint32_t maxTiles) {
    int32_t x = CMath::fint_(start.x - 0.5f);
    int32_t y = CMath::fint_(start.y - 0.5f);
    int32_t step;
    float frac;

    if (start.x > end.x) {
        step = -1;
        frac = static_cast<float>(x + 1) - start.x;
    } else {
        step = 1;
        frac = start.x - static_cast<float>(x);
    }

    float slope = (end.y - start.y) / fabs(end.x - start.x);
    float startY = start.y - frac * slope;
    float offset = 0.0f;
    int32_t endX = CMath::fint_(end.x - 0.5f) + step;

    if (x == endX) {
        return;
    }

    int32_t prevY = y;
    int32_t curY = y;

    do {
        if (curY != prevY) {
            tiles[(*tileCount)++] = x;
            tiles[(*tileCount)++] = prevY;
        }

        offset += slope;
        tiles[(*tileCount)++] = x;
        tiles[(*tileCount)++] = curY;
        x += step;
        prevY = curY;
        curY = CMath::fint_(offset + startY - 0.5f);
    } while (x != endX);

    if (curY != prevY && curY >= 0 && curY < this->m_liquidTiles.y) {
        tiles[(*tileCount)++] = x - step;
        tiles[(*tileCount)++] = curY;
    }
}

// VERIFY: 3612548b manual
// OFFSET: 0x007C9370
void CMapObjGroup::VectorIntersectLiquidDY(const C3Vector& start, const C3Vector& end, uint32_t* tileCount, uint8_t* tiles, uint32_t maxTiles) {
    int32_t x = CMath::fint_(start.x - 0.5f);
    int32_t y = CMath::fint_(start.y - 0.5f);
    int32_t step;
    float frac;

    if (start.y > end.y) {
        step = -1;
        frac = static_cast<float>(y + 1) - start.y;
    } else {
        step = 1;
        frac = start.y - static_cast<float>(y);
    }

    float slope = (end.x - start.x) / fabs(end.y - start.y);
    float startX = start.x - frac * slope;
    float offset = 0.0f;
    int32_t endY = CMath::fint_(end.y - 0.5f) + step;

    if (y == endY) {
        return;
    }

    int32_t prevX = x;
    int32_t curX = x;

    do {
        if (curX != prevX) {
            tiles[(*tileCount)++] = prevX;
            tiles[(*tileCount)++] = y;
        }

        offset += slope;
        tiles[(*tileCount)++] = curX;
        tiles[(*tileCount)++] = y;
        y += step;
        prevX = curX;
        curX = CMath::fint_(offset + startX - 0.5f);
    } while (y != endY);

    if (curX != prevX && curX >= 0 && curX < this->m_liquidTiles.x) {
        tiles[(*tileCount)++] = curX;
        tiles[(*tileCount)++] = y - step;
    }
}

// VERIFY: 8f1675f7 manual
// OFFSET: 0x007C8DD0
bool CMapObjGroup::VectorIntersectLiquidTiles(const C3Segment& seg, float* t, uint32_t flags, uint32_t* tileCount, uint8_t* tiles, World::TriData* triData, CMapObjDef* mapObjDef) {
    int32_t type = this->m_liquidType;

    if (!type || !this->m_liquidTiles.x || !this->m_liquidTiles.y) {
        return false;
    }

    if ((flags & 0x10000) && !(flags & 0x20000) && !(g_liquidTypeDB.GetRecord(type)->m_flags & 0x4)) {
        return false;
    }

    float dx = seg.t.x - seg.b.x;
    float dy = seg.t.y - seg.b.y;
    float dz = seg.t.z - seg.b.z;
    float invLength = 1.0f / sqrt(dx * dx + dz * dz + dy * dy);

    C3Ray ray;
    ray.origin = seg.b;
    ray.dir.x = dx * invLength;
    ray.dir.y = dy * invLength;
    ray.dir.z = invLength * dz;

    C3Vector* verts = this->m_liquidVertArray->m_verts.m_data;
    uint32_t tris[2][3];
    tris[0][0] = 0;
    tris[0][1] = this->m_liquidVerts.x + 1;
    tris[0][2] = this->m_liquidVerts.x;
    tris[1][0] = 0;
    tris[1][1] = 1;
    tris[1][2] = this->m_liquidVerts.x + 1;

    World::TriData::Batch* batch = nullptr;
    uint16_t* indices = nullptr;
    bool found = false;

    for (uint32_t i = 0; i < *tileCount; i += 2) {
        uint32_t tileX = tiles[0];
        uint32_t tileY = tiles[1];

        if ((this->m_liquidTileList[tileY * this->m_liquidTiles.x + tileX].m_flags & 0xF) != 0xF) {
            uint32_t base = tileX + tileY * this->m_liquidVerts.x;

            for (uint32_t j = 0; j < 2; j++) {
                uint32_t triIndices[3];
                triIndices[0] = base + tris[j][0];
                triIndices[1] = base + tris[j][1];
                triIndices[2] = base + tris[j][2];

                float hitT = 1.0f;

                if (!NTempest::Intersect(ray, verts, triIndices, &hitT, nullptr, 0.0099999998f)) {
                    continue;
                }

                hitT *= invLength;

                if (hitT >= *t || hitT < 0.0f) {
                    continue;
                }

                *t = hitT;

                if (!triData) {
                    found = true;
                    continue;
                }

                if (!found) {
                    batch = triData->AllocBatch();

                    if (batch) {
                        batch->m_def = mapObjDef;
                        batch->m_matrix = &mapObjDef->m_mat;
                        batch->m_vertexList = this->m_liquidVertArray->m_verts.m_data;
                        indices = triData->AllocIndices(3);
                        batch->m_indices = indices;
                    }
                }

                found = true;

                if (!batch || !batch->m_indices) {
                    continue;
                }

                batch->m_indexCount = 0;

                for (uint32_t k = 0; k < 3; k++) {
                    uint16_t index = base + tris[j][k];
                    indices[batch->m_indexCount++] = index;
                    batch->m_minVertexIndex = batch->m_minVertexIndex < index ? batch->m_minVertexIndex : index;
                    batch->m_maxVertexIndex = batch->m_maxVertexIndex > index ? batch->m_maxVertexIndex : index;
                }

                batch->m_faceCount++;
            }
        }

        tiles += 2;
    }

    return found;
}

// VERIFY: 4c425802 auto
// OFFSET: 0x007C9DD0
bool CMapObjGroup::VectorIntersectLiquid(const C3Segment& seg, float* t, uint32_t flags, World::TriData* triData, CMapObjDef* mapObjDef) {
    float sizeY = this->m_liquidTiles.y * 4.1666665f - 0.1f;
    sizeY = sizeY > 0.0f ? sizeY : 0.0f;
    float sizeX = 4.1666665f * this->m_liquidTiles.x - 0.1f;
    sizeX = sizeX > 0.0f ? sizeX : 0.0f;

    CAaBox box;
    box.b.x = this->m_liquidCorner.x;
    box.b.y = this->m_liquidCorner.y;
    box.b.z = this->m_liquidHeight;
    box.t.x = sizeX + this->m_liquidCorner.x;
    box.t.y = sizeY + this->m_liquidCorner.y;
    box.t.z = this->m_liquidMaxHeight;

    C3Vector start = seg.b;
    C3Vector end = seg.t;
    C3Vector dir;
    dir.x = end.x - start.x;
    dir.y = end.y - start.y;
    dir.z = end.z - start.z;

    float lengthSq = dir.z * dir.z + dir.y * dir.y + dir.x * dir.x;

    if (lengthSq > 2.3841858e-7f) {
        float invLength = 1.0f / sqrt(lengthSq);
        dir.x *= invLength;
        dir.y *= invLength;
        dir.z *= invLength;
    }

    if (!box.ContainsPoint(start)) {
        C3Ray ray;
        ray.origin = seg.b;
        ray.dir = dir;
        float rayT = 1.0f;

        if (!NTempest::Intersect(ray, box, &rayT, &start)) {
            return false;
        }
    }

    if (!box.ContainsPoint(end)) {
        C3Ray ray;
        ray.origin = end;
        dir.x = -dir.x;
        dir.y = -dir.y;
        dir.z = -dir.z;
        ray.dir = dir;
        float rayT = 1.0f;

        if (!NTempest::Intersect(ray, box, &rayT, &end)) {
            return false;
        }
    }

    uint32_t tileCount = 0;
    uint8_t tiles[0x800];

    start.x = (start.x - this->m_liquidCorner.x) * 0.24f;
    start.y = (start.y - this->m_liquidCorner.y) * 0.24f;
    start.z = start.z * 0.24f;
    end.x = (end.x - this->m_liquidCorner.x) * 0.24f;
    end.y = (end.y - this->m_liquidCorner.y) * 0.24f;
    end.z = 0.24f * end.z;

    float deltaX = end.x - start.x;
    float deltaY = fabs(end.y - start.y);

    if (deltaY < 2.3841858e-7f) {
        this->VectorIntersectLiquidSX(start, end, &tileCount, tiles, sizeof(tiles));
    } else if (fabs(deltaX) < 2.3841858e-7f) {
        this->VectorIntersectLiquidSY(start, end, &tileCount, tiles, sizeof(tiles));
    } else if (deltaY >= fabs(deltaX)) {
        this->VectorIntersectLiquidDY(start, end, &tileCount, tiles, sizeof(tiles));
    } else {
        this->VectorIntersectLiquidDX(start, end, &tileCount, tiles, sizeof(tiles));
    }

    return this->VectorIntersectLiquidTiles(seg, t, flags, &tileCount, tiles, triData, mapObjDef);
}

// VERIFY: 2533e68d manual
// OFFSET: 0x007C94B0
bool CMapObjGroup::GetLiquidTris(const CAaBox& box, uint32_t flags, World::TriData* triData, CMapObjDef* mapObjDef) {
    int32_t type = this->m_liquidType;

    if (!type || !this->m_liquidTiles.x || !this->m_liquidTiles.y) {
        return false;
    }

    if ((flags & 0x10000) && !(flags & 0x20000) && !(g_liquidTypeDB.GetRecord(type)->m_flags & 0x4)) {
        return false;
    }

    C3Vector points[2];
    points[0].x = box.b.x - this->m_liquidCorner.x;
    points[0].y = box.b.y - this->m_liquidCorner.y;
    points[0].z = box.b.z;
    points[1].x = box.t.x - this->m_liquidCorner.x;
    points[1].y = box.t.y - this->m_liquidCorner.y;
    points[1].z = box.t.z;

    CAaBox bounds = CAaBox::Bounding(points, 2);

    CiRect rect;
    rect.maxX = static_cast<int32_t>(floor(bounds.t.x * 0.24f));
    rect.maxY = static_cast<int32_t>(floor(bounds.t.y * 0.24f));
    rect.minY = static_cast<int32_t>(floor(bounds.b.y * 0.24f));
    rect.minX = static_cast<int32_t>(floor(bounds.b.x * 0.24f));

    if (rect.maxX < 0 || rect.maxY < 0 || rect.minX >= this->m_liquidTiles.x || rect.minY >= this->m_liquidTiles.y) {
        return false;
    }

    CiRect clip;
    clip.minY = 0;
    clip.minX = 0;
    clip.maxY = this->m_liquidTiles.y - 1;
    clip.maxX = this->m_liquidTiles.x - 1;
    rect &= clip;

    int32_t vertSkip = this->m_liquidVerts.x - rect.maxX + rect.minX - 2;
    int32_t height = rect.maxY - rect.minY;
    int32_t width = rect.maxX - rect.minX;
    uint32_t cullSize = (width + 2) * (height + 2);
    int32_t first = rect.minX + rect.minY * this->m_liquidVerts.x;

    if (cullSize > 0x2000) {
        return false;
    }

    uint8_t* cull = static_cast<uint8_t*>(alloca(cullSize));
    const C3Vector* vert = this->m_liquidVertArray->m_verts.m_data + first;
    uint8_t* cullOut = cull;

    for (int32_t y = rect.minY; y <= rect.maxY + 1; y++) {
        for (int32_t x = rect.minX; x <= rect.maxX + 1; x++) {
            *cullOut++ = CullLiquidVertexZ(box, *vert);
            vert++;
        }

        vert += vertSkip;
    }

    const C3Vector* tileVert = this->m_liquidVertArray->m_verts.m_data + first;
    const SMOLTile* tile = &this->m_liquidTileList[rect.minY * this->m_liquidTiles.x + rect.minX];
    int32_t tileSkip = this->m_liquidTiles.x - rect.maxX + rect.minX - 1;

    int32_t cullTris[2][3];
    cullTris[0][0] = 0;
    cullTris[0][1] = width + 3;
    cullTris[0][2] = width + 2;
    cullTris[1][0] = 0;
    cullTris[1][1] = 1;
    cullTris[1][2] = width + 3;

    int32_t vertTris[2][3];
    vertTris[0][0] = 0;
    vertTris[0][1] = this->m_liquidVerts.x + 1;
    vertTris[0][2] = this->m_liquidVerts.x;
    vertTris[1][0] = 0;
    vertTris[1][1] = 1;
    vertTris[1][2] = this->m_liquidVerts.x + 1;

    World::TriData::Batch* batch = nullptr;
    uint16_t* indices = nullptr;
    bool found = false;
    const uint8_t* cullTile = cull;

    for (int32_t y = rect.minY; y <= rect.maxY; y++) {
        for (int32_t x = rect.minX; x <= rect.maxX; x++) {
            if ((tile->m_flags & 0xF) != 0xF) {
                for (uint32_t j = 0; j < 2; j++) {
                    if (cullTile[cullTris[j][0]] & cullTile[cullTris[j][2]] & cullTile[cullTris[j][1]]) {
                        continue;
                    }

                    if (!found) {
                        batch = triData->AllocBatch();

                        if (batch) {
                            batch->m_def = mapObjDef;
                            batch->m_matrix = &mapObjDef->m_mat;
                            batch->m_vertexList = this->m_liquidVertArray->m_verts.m_data;
                            indices = triData->AllocIndices(6 * (width + 1) * (height + 1));
                            batch->m_indices = indices;
                        }
                    }

                    found = true;

                    if (!batch || !batch->m_indices) {
                        continue;
                    }

                    for (uint32_t k = 0; k < 3; k++) {
                        uint16_t index = (tileVert - this->m_liquidVertArray->m_verts.m_data) + vertTris[j][k];
                        indices[batch->m_indexCount++] = index;
                        batch->m_minVertexIndex = batch->m_minVertexIndex < index ? batch->m_minVertexIndex : index;
                        batch->m_maxVertexIndex = batch->m_maxVertexIndex > index ? batch->m_maxVertexIndex : index;
                    }

                    batch->m_faceCount++;
                }
            }

            tileVert++;
            tile++;
            cullTile++;
        }

        tileVert += tileSkip + 1;
        tile += tileSkip;
        cullTile++;
    }

    return found;
}

// VERIFY: 22aa2402 manual
// OFFSET: 0x007CAB70
bool CMapObjGroup::GetLiquidTris(CFrustum& frustum, uint32_t flags, World::TriData* triData, CMapObjDef* mapObjDef) {
    int32_t type = this->m_liquidType;

    if (!type || !this->m_liquidTiles.x || !this->m_liquidTiles.y) {
        return false;
    }

    if ((flags & 0x10000) && !(flags & 0x20000) && !(g_liquidTypeDB.GetRecord(type)->m_flags & 0x4)) {
        return false;
    }

    C3Vector offset;
    offset.x = -this->m_liquidCorner.x;
    offset.y = -this->m_liquidCorner.y;
    offset.z = -this->m_liquidCorner.z;

    CWFrustum localFrustum;
    static_cast<CFrustum&>(localFrustum) = frustum;
    localFrustum.Translate(offset);

    CAaBox bounds = CAaBox::Bounding(localFrustum.corners, 8);

    CiRect rect;
    rect.maxX = static_cast<int32_t>(floor(bounds.t.x * 0.24f));
    rect.maxY = static_cast<int32_t>(floor(bounds.t.y * 0.24f));
    rect.minY = static_cast<int32_t>(floor(bounds.b.y * 0.24f));
    rect.minX = static_cast<int32_t>(floor(bounds.b.x * 0.24f));

    if (rect.maxX < 0 || rect.maxY < 0 || rect.minX >= this->m_liquidTiles.x || rect.minY >= this->m_liquidTiles.y) {
        return false;
    }

    CiRect clip;
    clip.minY = 0;
    clip.minX = 0;
    clip.maxY = this->m_liquidTiles.y - 1;
    clip.maxX = this->m_liquidTiles.x - 1;
    rect &= clip;

    int32_t vertSkip = this->m_liquidVerts.x - rect.maxX + rect.minX - 2;
    int32_t height = rect.maxY - rect.minY;
    int32_t width = rect.maxX - rect.minX;
    uint32_t cullSize = (width + 2) * (height + 2);
    int32_t first = rect.minX + rect.minY * this->m_liquidVerts.x;

    if (cullSize > 0x2000) {
        return false;
    }

    uint8_t* cull = static_cast<uint8_t*>(alloca(cullSize));
    const C3Vector* vert = this->m_liquidVertArray->m_verts.m_data + first;
    uint8_t* cullOut = cull;

    for (int32_t y = rect.minY; y <= rect.maxY + 1; y++) {
        for (int32_t x = rect.minX; x <= rect.maxX + 1; x++) {
            frustum.Cull(*vert, *cullOut);
            vert++;
            cullOut++;
        }

        vert += vertSkip;
    }

    const C3Vector* tileVert = this->m_liquidVertArray->m_verts.m_data + first;
    const SMOLTile* tile = &this->m_liquidTileList[rect.minY * this->m_liquidTiles.x + rect.minX];
    int32_t tileSkip = this->m_liquidTiles.x - rect.maxX + rect.minX - 1;

    int32_t cullTris[2][3];
    cullTris[0][0] = 0;
    cullTris[0][1] = width + 3;
    cullTris[0][2] = width + 2;
    cullTris[1][0] = 0;
    cullTris[1][1] = 1;
    cullTris[1][2] = width + 3;

    int32_t vertTris[2][3];
    vertTris[0][0] = 0;
    vertTris[0][1] = this->m_liquidVerts.x + 1;
    vertTris[0][2] = this->m_liquidVerts.x;
    vertTris[1][0] = 0;
    vertTris[1][1] = 1;
    vertTris[1][2] = this->m_liquidVerts.x + 1;

    World::TriData::Batch* batch = nullptr;
    uint16_t* indices = nullptr;
    bool found = false;
    const uint8_t* cullTile = cull;

    for (int32_t y = rect.minY; y <= rect.maxY; y++) {
        for (int32_t x = rect.minX; x <= rect.maxX; x++) {
            if ((tile->m_flags & 0xF) != 0xF) {
                for (uint32_t j = 0; j < 2; j++) {
                    if (cullTile[cullTris[j][0]] & cullTile[cullTris[j][2]] & cullTile[cullTris[j][1]]) {
                        continue;
                    }

                    if (!found) {
                        batch = triData->AllocBatch();

                        if (batch) {
                            batch->m_def = mapObjDef;
                            batch->m_matrix = &mapObjDef->m_mat;
                            batch->m_vertexList = this->m_liquidVertArray->m_verts.m_data;
                            indices = triData->AllocIndices(6 * (width + 1) * (height + 1));
                            batch->m_indices = indices;
                        }
                    }

                    found = true;

                    if (!batch || !batch->m_indices) {
                        continue;
                    }

                    for (uint32_t k = 0; k < 3; k++) {
                        uint16_t index = (tileVert - this->m_liquidVertArray->m_verts.m_data) + vertTris[j][k];
                        indices[batch->m_indexCount++] = index;
                        batch->m_minVertexIndex = batch->m_minVertexIndex < index ? batch->m_minVertexIndex : index;
                        batch->m_maxVertexIndex = batch->m_maxVertexIndex > index ? batch->m_maxVertexIndex : index;
                    }

                    batch->m_faceCount++;
                }
            }

            tileVert++;
            tile++;
            cullTile++;
        }

        tileVert += tileSkip + 1;
        tile += tileSkip;
        cullTile++;
    }

    return found;
}

// VERIFY: 121a8eb8 manual
// OFFSET: 0x007CA110
bool CMapObjGroup::GetLiquidFacets(const CAaBox& box, uint32_t flags, World::FacetData* facets, CMapObjDef* mapObjDef) {
    World::TriData::statusFlags = 0;
    World::TriData::nBatches = 0;
    World::TriData::faceIndexCursor = 0;
    World::TriData::indexCursor = 0;
    World::TriData::unk_CB7538 = 0;

    World::TriData triData;
    this->GetLiquidTris(box, flags, &triData, mapObjDef);

    WGUID guid;
    guid.m_guidLow = mapObjDef->unk_0148;
    guid.m_guidHigh = mapObjDef->unk_014C;
    World::TriDataToFacetData(&triData, facets, guid);

    return World::TriData::nBatches != 0;
}
