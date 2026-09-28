#include "MapChunk.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"

// STATIC: 0x00D25488
float CMapChunk::s_geoToTex;

// STATIC: 0x00D25498
C3Vector CMapChunk::vertexList[145];

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
