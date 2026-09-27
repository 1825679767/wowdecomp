#include <cstdint>
#include <cstring>
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Event/CSizeEvent.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Scrn/Types.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Services/Types.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/LoadingScreen.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"
#include "WoW/Source/Ui/SpellBookFrame.h"

// STATIC: 0x00AB6470
static CSizeEvent s_sizeEvent;

// STATIC: 0x00B2FE60
static CGxShader* s_pixelShader[1];

// STATIC: 0x00B2FE64
static CGxShader* s_vertexShader[2];

// STATIC: 0x00B2FE74
static CTexture* s_dynamicElementsTexture;

// STATIC: 0x00B2FE78
static CTexture* s_dynamicBackgroundTextures[12];

// STATIC: 0x00B2FEA8
static CTexture* s_simpleBackgroundTexture;

// STATIC: 0x00B2FECC
static CTexture* s_textures[2];

// STATIC: 0x00B2FED4
static HLAYER s_loadingScreenLayer;

// STATIC: 0x00B2FEE0
static HTEXTFONT s_textFont;

// STATIC: 0x00B2FEE4
static CGxString* s_tipStr;

// STATIC: 0x00B2FEE8
static const char* s_gameTip;

// STATIC: 0x00B2FEEC
static CGxStringBatch* s_batch;

// STATIC: 0x00B2FEF0
static int32_t s_sizeEventPosted;

// STATIC: 0x00B2FF00
static C3Vector s_tileVertices[12][4];

// STATIC: 0x00B30140
static C2Vector s_tileTexCoords[12][4];

// STATIC: 0x00B302C0
static TSFixedArray<DYNAMICELEMENTVERT> s_dynamicElements;

// STATIC: 0x00B302D0
static CLoadingScreenShipInfo<TaxiPathNodeRec> s_shipInfo;

// VERIFY: 30cfdd0d manual
// OFFSET: 0x00407C50
static void BuildTiles() {
    for (int32_t row = 0; row < 3; row++) {
        for (int32_t col = 0; col < 4; col++) {
            int32_t left = col * 256;
            int32_t top = row * 256;
            int32_t right = left + 256;
            int32_t bottom = top + 256;

            if (right > 1002) {
                right = 1002;
            }

            if (bottom > 668) {
                bottom = 668;
            }

            int32_t tile = row * 4 + col;
            s_tileVertices[tile][0] = C3Vector(left * (1.0f / 1002.0f), 1.0f - bottom * (1.0f / 668.0f), 0.0f);
            s_tileVertices[tile][1] = C3Vector(right * (1.0f / 1002.0f), 1.0f - bottom * (1.0f / 668.0f), 0.0f);
            s_tileVertices[tile][2] = C3Vector(left * (1.0f / 1002.0f), 1.0f - top * (1.0f / 668.0f), 0.0f);
            s_tileVertices[tile][3] = C3Vector(right * (1.0f / 1002.0f), 1.0f - top * (1.0f / 668.0f), 0.0f);

            float u = 1.0f;
            float v = 1.0f;

            if (col == 3) {
                u = 234.0f / 256.0f;
            }

            if (row == 2) {
                v = 156.0f / 256.0f;
            }

            s_tileTexCoords[tile][0] = C2Vector(0.0f, v);
            s_tileTexCoords[tile][1] = C2Vector(u, v);
            s_tileTexCoords[tile][2] = C2Vector(0.0f, 0.0f);
            s_tileTexCoords[tile][3] = C2Vector(u, 0.0f);
        }
    }
}

// VERIFY: b60ab174 auto
// OFFSET: 0x0040B2B0
void LoadingScreenInitialize() {
    g_theGxDevicePtr->ShaderCreate(s_vertexShader, GxSh_Vertex, "Shaders\\Vertex", "UI", 2);
    g_theGxDevicePtr->ShaderCreate(s_pixelShader, GxSh_Pixel, "Shaders\\Pixel", "UI", 1);
    s_shipInfo.Initialize(&g_taxiPathNodeDB, g_taxiPathDB.m_maxID + 1);
    BuildTiles();
}

// VERIFY: 91ed5224 auto
// OFFSET: 0x00407AB0
static int32_t SizeEvent(const void* data, void* param) {
    memcpy(&s_sizeEvent, data, sizeof(s_sizeEvent));
    s_sizeEventPosted = 1;
    return 0;
}

// VERIFY: 089a3404 auto
// OFFSET: 0x008E5250
static int32_t SwallowEvent(const void* data, void* param) {
    return 0;
}

// VERIFY: 2414bf03 auto
// OFFSET: 0x00407BD0
static void LoadingScreenDisableEvents() {
    EventUnregister(EVENT_ID_CHAR, SwallowEvent);
    EventUnregister(EVENT_ID_IME, SwallowEvent);
    EventUnregister(EVENT_ID_KEYDOWN, SwallowEvent);
    EventUnregister(EVENT_ID_KEYDOWN_REPEATING, SwallowEvent);
    EventUnregister(EVENT_ID_MOUSEDOWN, SwallowEvent);
    EventUnregister(EVENT_ID_MOUSEMOVE, SwallowEvent);
    EventUnregister(EVENT_ID_SIZE, SizeEvent);

    if (s_sizeEventPosted) {
        EventQueuePost(EventGetCurrentContext(), EVENT_ID_SIZE, &s_sizeEvent, sizeof(s_sizeEvent));
    }
}

// VERIFY: 69f3b93a manual
// OFFSET: 0x004094E0
static void ClearDynamicData() {
    if (s_dynamicElementsTexture) {
        HandleClose(reinterpret_cast<HOBJECT>(s_dynamicElementsTexture));
    }

    s_dynamicElementsTexture = nullptr;

    for (uint32_t i = 0; i < sizeof(s_dynamicBackgroundTextures) / sizeof(s_dynamicBackgroundTextures[0]); i++) {
        if (s_dynamicBackgroundTextures[i]) {
            HandleClose(reinterpret_cast<HOBJECT>(s_dynamicBackgroundTextures[i]));
        }

        s_dynamicBackgroundTextures[i] = nullptr;
    }

    s_dynamicElements.Clear();
}

// VERIFY: ccd42e4f manual
// OFFSET: 0x00409550
static void LoadingScreenDisable() {
    if (s_loadingScreenLayer) {
        HandleClose(reinterpret_cast<HOBJECT>(s_loadingScreenLayer));
    }

    s_loadingScreenLayer = nullptr;

    for (uint32_t i = 0; i < sizeof(s_textures) / sizeof(s_textures[0]); i++) {
        if (s_textures[i]) {
            HandleClose(reinterpret_cast<HOBJECT>(s_textures[i]));
        }

        s_textures[i] = nullptr;
    }

    if (s_simpleBackgroundTexture) {
        HandleClose(reinterpret_cast<HOBJECT>(s_simpleBackgroundTexture));
    }

    s_simpleBackgroundTexture = nullptr;

    LoadingScreenDisableEvents();
    ClearDynamicData();

    s_gameTip = nullptr;

    if (s_batch) {
        GxuFontDestroyBatch(s_batch);
    }

    s_batch = nullptr;

    if (s_tipStr) {
        GxuFontDestroyString(s_tipStr);
    }

    s_tipStr = nullptr;

    if (s_textFont) {
        HandleClose(reinterpret_cast<HOBJECT>(s_textFont));
    }

    s_textFont = nullptr;

    SI2::StopAllMusic();
    SI2::StopZoneAmbience();

    if (SFile::IsStreamingMode()) {
        CGSpellBook::MakeKnownSpellModelsLocal();
    }
}

// VERIFY: b183edd8 auto
// OFFSET: 0x00409620
void LoadingScreenShutdown() {
    LoadingScreenDisable();

    for (uint32_t i = 0; i < sizeof(s_vertexShader) / sizeof(s_vertexShader[0]); i++) {
        if (s_vertexShader[i]) {
            g_theGxDevicePtr->ShaderDestroy(s_vertexShader[i]);
        }
    }

    if (s_pixelShader[0]) {
        g_theGxDevicePtr->ShaderDestroy(s_pixelShader[0]);
    }
}
