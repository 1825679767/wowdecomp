# Native modular WXL for WotLK x64

Active goal: integrate WXL into the native WotLK x64 source. DX11 is deferred at the human's request. Existing native D3D9/D3D9Ex/OpenGL remain the renderer. Native fog, liquids, weather and postprocessing are not replaced by WXL.

WotLK base: SatyPardus/wowdecomp cd29da38b9162419ee0fa29c6b16e07e10f6c4f0. Portable WXL vendor provenance is recorded in VENDORED-SOURCE.json and APPEARANCE-VENDOR-SOURCE.json. No executable addresses, injected host, Win32 proxy or binary signature hooks are used in this integration.

## Module boundaries

Each module has its own object-library build target and source directory. WxlNative/WXL::Native collects those objects once into a static library linked into the executable. This avoids circular static-archive resolution while retaining separate compilation and stable public module interfaces. This is not a dynamic DLL plugin ABI.

| Module ID | Target / alias | Responsibility |
| --- | --- | --- |
| runtime | WxlRuntime / WXL::Runtime | feature settings, logging, immutable module metadata |
| storage | WxlStorage / WXL::Storage | bounded native mounted-file reads |
| db2 | WxlDB2 / WXL::DB2 | WDC tables, FDID paths, material texture IDs and appearance schemas |
| blp | WxlBlp / WXL::BLP | modern texture decoding/transcode |
| materials | WxlMaterials / WXL::Materials | source mesh material preservation; native Gx blend factors |
| particles | WxlParticles / WXL::Particles | emitter normalization, EXP2 multiplier bake and neutral EXPT |
| m2 | WxlM2 / WXL::M2 | native model/SKIN conversion, skeletons, animation and owned companion lifetime |
| appearances | WxlAppearances / WXL::Appearances | component-owned atlas/mips, equipment textures, geosets and whole sheets |
| protocol | WxlProtocol / WXL::Protocol | bounded custom packet transport, session lifecycle and Lua APIs |

Public headers remain under WXL/. Native call sites use those headers; implementations live under WXL/modules/. DB2 no longer depends on a model adapter just to read files, and BLP preprocessing is separate from model loading. Narrow native Gx, Model2, character and network seams stay with their native owners.

## Feature controls

All implemented features default ON. CMake options: WXL_ENABLE_NATIVE (master), WXL_ENABLE_ASSETS, WXL_ENABLE_DB2, WXL_ENABLE_BLP, WXL_ENABLE_MATERIALS, WXL_ENABLE_PARTICLES, WXL_ENABLE_APPEARANCES, WXL_ENABLE_PROTOCOL. Options control feature availability; interface implementations remain linked so native call sites have defined disabled behavior.

Runtime CVars: wxlModernAssets, wxlRetailDb2, wxlModernBlp, wxlCustomMaterials, wxlParticles, wxlRetailAppearances, wxlCustomOpcodes. Asset/DB changes require restart; model changes require reload. Appearances additionally require assets, DB2 and BLP; material/particle adaptation requires assets. Disabling dependencies suppresses the dependent path without changing its saved setting.

C++ ModuleCount/QueryModule expose version1 module descriptors. Lua WXL_GetNativeModules() returns id, apiVersion, dependencies and effective enabled state. NativeProtocol.md documents binary custom packet send/pop APIs; transport readiness is not a server acknowledgement or complete feature parser.

## Integrated native paths

CM2Shared converts modern data before native relocation and owns converted SKIN companions until destruction. Per-section palettes use the actual backend constant budget. Retail high-word index offsets are consumed throughout upload/render/intersection; the legacy uint16-count optimizer is bypassed for modern topology over65535 indices rather than truncating it.

Sized sync/async BLP sources keep transcode buffers alive through upload. The appearance module joins mounted retail customization/material/layout tables, checks exact model identity, composes private atlas sheets and equipment layers, applies geosets and owns replaceable textures with native teardown. Creation preferences are sized from mounted race IDs instead of the former22-race array.

Mesh blend7 ONE/INV_SRC_ALPHA remains separate from particle Screen INV_DEST_COLOR/ONE. EXP2 constant RGB/alpha multipliers are baked into private native keys. Neutral EXPT accepts only exact0/1/1 triples per emitter with absent or exact neutral TXAC, based on the documented Whiteout source contract; mixed EXP2/EXPT, unknown values and malformed rows refuse without publishing altered model bytes.

## Remaining integration

- Retail attached customization models and remaining race-specific authoring policies.
- Retail choice enumeration/cycling/validation/initialization and server choice/save agreement.
- Custom packet feature consumers, including appearance hello/prepare/create persistence.
- Remaining material shader and particle-layer/cutoff families; broader model source compatibility.
- Remaining WXL client extensions, UI, WMO/ADT and related modules, following the same module interfaces.
- DX11 is a separate deferred task; DX11-PORT.md is historical dependency research, not a prerequisite for this x64 WXL build.

Compilation and source binding do not imply gameplay acceptance. Keep installed clients and rollback separate; no new Downloads folders or repeated gameplay probe loops are part of this work.
