#ifndef ENGINE_SOURCE_GX_CGXDEVICE_CGXDEVICE_H
#define ENGINE_SOURCE_GX_CGXDEVICE_CGXDEVICE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/CGxDevice/Types.h"
#include "Engine/Source/Gx/CGxFormat.h"
#include "Engine/Source/Gx/CGxGammaRamp.h"
#include "Engine/Source/Gx/CGxMatrixStack.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/c44matrix.h"
#include "Engine/Source/Tempest/c4plane.h"
#include "Engine/Source/Tempest/cimvector.h"
#include "Storm/H/stpl.h"
#include "WoW/Source/Types.h"
#include "WoW/Source/WorldClient/Types.h"
#include "WoW/Source/WorldClient/World.h"
#include "_Triage/CRect.h"
#include "_Triage/EmergencyMem.h"

class CGxAppRenderState;
struct CGxBuf;
struct CGxPool;
struct CGxQuery;
struct CGxShader;
class CGxStateBom;
struct CGxTex;
struct CImVector;
struct HASHKEY_STR;

class CGxApiLight {
    public:
    /* 0000 */ C4Vector m_dir;
    /* 0010 */ C3Vector m_ambColor;
    /* 001C */ C3Vector m_dirColor;
    /* 0028 */ C3Vector m_specColor;
    /* 0034 */ float m_constantAttenuation;
    /* 0038 */ float m_linearAttenuation;
    /* 003C */ float m_quadraticAttenuation;
    /* 0040 */ int32_t m_enable;
    /* 0044 */ uint32_t m_flags;
};

class CGxStateBom {
    public:
    /* 0000 */ uint8_t m_data[12];
    /* 000C */ int32_t m_filler;
};

class CGxAppRenderState {
    public:
    /* 0000 */ CGxStateBom m_value;
    /* 0010 */ uint32_t m_stackDepth;
    /* 0014 */ int32_t m_dirty;
};

class CGxDevice {
    public:
    struct TextureTarget {
        /* 0000 */ CGxTex* m_texture;
        /* 0004 */ uint32_t m_plane;
        /* 0008 */ void* m_apiSpecific;
    };

    /* 0004 */ TSGrowableArray<uint32_t> m_pushedStates;
    /* 0014 */ TSGrowableArray<uint32_t> m_stackOffsets;
    /* 0024 */ TSGrowableArray<uint32_t> m_dirtyStates;
    /* 0034 */ EGxPrim m_primType;
    /* 0038 */ int32_t m_indexLocked;
    /* 003C */ int32_t m_vertexLocked;
    /* 0040 */ int32_t m_inBeginEnd;
    /* 0044 */ C3Vector m_primVertex;
    /* 0050 */ C2Vector m_primTexCoord [8];
    /* 0090 */ C3Vector m_primNormal;
    /* 009C */ CImVector m_primColor;
    /* 00A0 */ TSGrowableArray<uint32_t> m_primVertexArray;
    /* 00B0 */ TSGrowableArray<uint32_t> m_primTexCoordArray [8];
    /* 0130 */ TSGrowableArray<uint32_t> m_primNormalArray;
    /* 0140 */ TSGrowableArray<uint32_t> m_primColorArray;
    /* 0150 */ TSGrowableArray<uint32_t> m_primIndexArray;
    /* 0160 */ uint32_t m_primMask;
    /* 0164 */ CRect m_defWindowRect;
    /* 0174 */ CRect m_curWindowRect;
    /* 0184 */ TSGrowableArray<uint32_t> m_deviceRestoredCallbacks;
    /* 0194 */ TSGrowableArray<uint32_t> m_textureRecreationCallbacks;
    /* 01A4 */ TSGrowableArray<uint32_t> m_stereoChangedCallbacks;
    /* 01B4 */ EGxApi m_api;
    /* 01B8 */ uint32_t m_cpuFeatures;
    /* 01BC */ CGxFormat m_format;
    /* 0214 */ CGxCaps m_caps;
    /* 0350 */ uint32_t m_baseMipLevel;
    /* 0354 */ CGxGammaRamp m_gammaRamp;
    /* 0954 */ CGxGammaRamp m_systemGammaRamp;
    /* 0F54 */ int32_t (__stdcall* m_windowProc )(void* window, uint32_t message, uintptr_t wparam, intptr_t lparam);
    /* 0F58 */ int32_t m_context;
    /* 0F5C */ int32_t m_intF5C;
    /* 0F60 */ int32_t m_windowVisible;
    /* 0F64 */ int32_t m_windowFocus;
    /* 0F68 */ int32_t m_frameCount;
    /* 0F6C */ int32_t m_viewportDirty;
    /* 0F70 */ CBoundingBox m_viewport;
    /* 0F88 */ C44Matrix m_projection;
    /* 0FC8 */ C44Matrix m_projNative;
    /* 1008 */ CGxMatrixStack m_xforms [11];
    /* 1C10 */ CGxMatrixStack m_texGen [8];
    /* 24D0 */ uint32_t m_clipPlaneMask;
    /* 24D4 */ C4Plane m_clipPlane [6];
    /* 2534 */ int32_t m_scissorRectDirty;
    /* 2538 */ CRect m_scissorRect;
    /* 2548 */ CGxApiLight m_lights [4];
    /* 2668 */ TSHashObject<CGxShader, HASHKEY_STR> m_shaderList [10];
    /* 2758 */ uint32_t m_appMasterEnables;
    /* 275C */ uint32_t m_hwMasterEnables;
    /* 2760 */ TSList<CGxPool> m_poolList;
    /* 276C */ uint32_t unk_276C [3];
    /* 2778 */ CGxBuf* m_bufLocked [2];
    /* 2780 */ CGxPool* m_vertexPool;
    /* 2784 */ CGxPool* m_indexPool;
    /* 2788 */ CGxBuf* m_streamBufs [2];
    /* 2790 */ CGxVertexAttrib m_primVertexFormatAttrib [14];
    /* 2870 */ CGxBuf* m_primVertexFormatBuf [14];
    /* 28A8 */ uint32_t m_primVertexMask;
    /* 28AC */ uint32_t m_primVertexDirty;
    /* 28B0 */ EGxVertexBufferFormat m_primVertexFormat;
    /* 28B4 */ CGxBuf* m_primVertexBuf;
    /* 28B8 */ uint32_t m_primVertexSize;
    /* 28BC */ CGxBuf* m_primIndexBuf;
    /* 28C0 */ int32_t m_primIndexDirty;
    /* 28C4 */ EmergencyMem m_emergencyMem [2];
    /* 28EC */ TSFixedArray<CGxAppRenderState> m_appRenderStates;
    /* 28F8 */ TSFixedArray<CGxStateBom> m_hwRenderStates;
    /* 2904 */ TSList<CGxTex> m_textures;
    /* 2910 */ CGxDevice::TextureTarget m_textureTarget [2];
    /* 2928 */ TSExplicitList<CGxQuery> m_queryList;
    /* 2934 */ int32_t m_scrShotClick;
    /* 2938 */ uint32_t m_scrShotWidth;
    /* 293C */ uint32_t m_scrShotHeight;
    /* 2940 */ TSGrowableArray<CImVector> m_scrShotPixels;
    /* 2950 */ int32_t m_cursorVisible;
    /* 2954 */ int32_t m_hwCursor;
    /* 2958 */ uint32_t m_cursorHotspotX;
    /* 295C */ uint32_t m_cursorHotspotY;
    /* 2960 */ uint32_t m_cursor [1024];
    /* 3960 */ CGxTex* m_cursorTexture;
    /* 3964 */ float m_cursorDepth;

    virtual ~CGxDevice();
};

#endif
