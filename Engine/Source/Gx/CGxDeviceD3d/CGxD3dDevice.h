#ifndef ENGINE_SOURCE_GX_CGXDEVICED3D_CGXD3DDEVICE_H
#define ENGINE_SOURCE_GX_CGXDEVICED3D_CGXD3DDEVICE_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include <d3d9.h>
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Storm/H/stpl.h"

struct CGxTex;

class CGxDeviceD3d : public CGxDevice {
    public:
    struct GxVertexDecl {
        /* 0000 */ uint32_t m_count;
        /* 0004 */ D3DVERTEXELEMENT9 m_elements [15];
        /* 007C */ IDirect3DVertexDeclaration9* m_d3dDecl;
    };

    /* 3968 */ void* m_hwnd;
    /* 396C */ uint16_t m_hwndClass;
    /* 396E */ uint16_t unk_396E;
    /* 3970 */ int32_t m_ownhwnd;
    /* 3974 */ void* m_d3dLib;
    /* 3978 */ LPDIRECT3D9 m_d3d;
    /* 397C */ LPDIRECT3DDEVICE9 m_d3dDevice;
    /* 3980 */ D3DCAPS9 m_d3dCaps;
    /* 3AB0 */ int32_t m_d3dIsHwDevice;
    /* 3AB4 */ int32_t m_d3dNVAPI;
    /* 3AB8 */ uint32_t m_d3dStereoEnabled;
    /* 3ABC */ uint32_t m_d3dStereoRestore;
    /* 3AC0 */ uint32_t m_d3dStereoHandle;
    /* 3AC4 */ float m_d3dStereoConvergence;
    /* 3AC8 */ float m_d3dStereoSeparation;
    /* 3ACC */ int32_t m_d3dStereoDirty;
    /* 3AD0 */ TSGrowableArray<CGxDeviceD3d::GxVertexDecl> m_gxVertexDecl;
    /* 3AE0 */ LPDIRECT3DVERTEXDECLARATION9 m_d3dVertexDecl [14];
    /* 3B18 */ D3DDISPLAYMODE m_desktopDisplayMode;
    /* 3B28 */ int32_t m_inScene;
    /* 3B2C */ int32_t m_ambientOnlyMode;
    /* 3B30 */ D3DFORMAT m_devDepthFormat;
    /* 3B34 */ D3DFORMAT m_devAdapterFormat;
    /* 3B38 */ LPDIRECT3DSURFACE9 m_defDepthStencilSurface;
    /* 3B3C */ LPDIRECT3DSURFACE9 m_defColorSurface;
    /* 3B40 */ LPDIRECT3DSURFACE9 m_defDepthSurface;
    /* 3B44 */ LPDIRECT3DSURFACE9 m_surface3B44;
    /* 3B48 */ LPDIRECT3DQUERY9 m_eventQuery;
    /* 3B4C */ int32_t m_hwCursorDirty;
    /* 3B50 */ LPDIRECT3DTEXTURE9 m_hwCursorTexture;
    /* 3B54 */ LPDIRECT3DSURFACE9 m_hwCursorBitmap;
    /* 3B58 */ CGxTex* m_texture3B58;
    /* 3B5C */ LPDIRECT3DVERTEXDECLARATION9 m_d3dCurrentVertexDecl;
    /* 3B60 */ LPDIRECT3DINDEXBUFFER9 m_d3dCurrentIndexBuf;
    /* 3B64 */ LPDIRECT3DVERTEXBUFFER9 m_d3dVertexStreamBuf [8];
    /* 3B84 */ uint32_t m_d3dVertexStreamOfs [8];
    /* 3BA4 */ uint32_t m_d3dVertexStreamStride [8];
    /* 3BC4 */ uint32_t m_deviceStates [182];
    /* 3E9C */ bool m_texEnabled [8];
};

#endif
