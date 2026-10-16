# Native WXL integration

Base: SatyPardus/wowdecomp main cd29da38b9162419ee0fa29c6b16e07e10f6c4f0, fetched16October2026. This is the WotLK source tree. The developer's WotLK/Cataclysm hybrid branch remains needed for the existing client's DX11/native effect family; this tree currently contains D3D9/D3D9Ex/OpenGL only. No replacement of the user client is implied.

WxlNative is linked statically into the source executable. Portable WXL M2/BLP/DB2 code is vendored with exact original provenance in VENDORED-SOURCE.json and GPL licenses. No old addresses, signature scans, injection, DLL module host or Win32 proxy are part of this source integration.

Native entry points:
- CM2Shared::FinishLoading converts MD21 to native data before M2Init; the native shared object owns converted companions until destruction.
- CM2Shared::FinishLoadingSkinProfile consumes the exact prepared companion before native fixup/geometry initialization.
- Existing native material and particle constructors/render passes preserve mesh blend7 ONE/INV_SRC_ALPHA independently of particle Screen INV_DEST_COLOR/ONE. All backend blend tables are extended using native enums.
- Sized CBLPFile sources transcode encoding3 to BC3; synchronous and asynchronous texture pumps pass actual byte sizes and preserve the existing upload lifetime.
- FDID path tables use the existing WXL WDC reader directly through mounted native SFile storage.

CMake: WXL_ENABLE_NATIVE (master), WXL_ENABLE_ASSETS, WXL_ENABLE_BLP, WXL_ENABLE_MATERIALS, WXL_ENABLE_PARTICLES (default ON). Runtime CVars: wxlModernAssets, wxlModernBlp, wxlCustomMaterials, wxlParticles. Changes require restart/model reload as indicated by client help; disabled features use existing native paths. The diagnostic build enables WOWDECOMP_DISABLE_SIGNATURE_CHECKS to load custom patch/FrameXML data; no account actions or archive changes are performed.

This is the first native integration batch, not full WXL parity. Existing high-palette/high-index/custom shader restrictions of the byte converter remain. WDC5/material graph/race UI/forms/equipment/persistence/world module porting and hybrid renderer merge remain. Native fog/liquid/weather/postprocess implementations are not imported from WXL. No game test loops or new Downloads folder.

Palette admission follows the active native backend's shader-constant capacity (the same calculation as CM2Shared::Initialize), capped at the existing converter's75-bone limit. D3D9 is not assumed to have the former hybrid DX11 budget.
