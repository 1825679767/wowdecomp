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

Runtime CVars: wxlModernAssets, wxlRetailDb2, wxlModernBlp, wxlCustomMaterials, wxlParticles, wxlRetailAppearances, wxlCustomOpcodes, wxlAppearanceSave. Asset/DB changes require restart; model changes require reload. Appearances additionally require assets, DB2 and BLP; material/particle adaptation requires assets. Disabling dependencies suppresses the dependent path without changing its saved setting.

C++ ModuleCount/QueryModule expose version1 module descriptors. Lua WXL_GetNativeModules() returns id, apiVersion, dependencies and effective enabled state. NativeProtocol.md documents binary custom packet send/pop APIs; transport readiness is not a server acknowledgement or complete feature parser.

## Integrated native paths

CM2Shared converts modern data before native relocation and owns converted SKIN companions until destruction. Per-section palettes use the actual backend constant budget. Retail high-word index offsets are consumed throughout upload/render/intersection; the legacy uint16-count optimizer is bypassed for modern topology over65535 indices rather than truncating it.

Sized sync/async BLP sources keep transcode buffers alive through upload. The appearance module joins mounted retail customization/material/layout tables, checks exact model identity, composes private atlas sheets and equipment layers, applies geosets and owns replaceable textures with native teardown. Creation preferences are sized from mounted race IDs instead of the former22-race array.

Mesh blend7 ONE/INV_SRC_ALPHA remains separate from particle Screen INV_DEST_COLOR/ONE. EXP2 constant RGB/alpha multipliers are baked into private native keys. Neutral EXPT accepts only exact0/1/1 triples per emitter with absent or exact neutral TXAC, based on the documented Whiteout source contract; mixed EXP2/EXPT, unknown values and malformed rows refuse without publishing altered model bytes.

## Remaining integration

- Remaining race-specific authoring policies and children with subset/reordered skeletons.
- Authenticated server acceptance of appearance hello/prepare/create and persistence; remaining feature packet consumers. Area-loot/garrison/spell charges have native consumers.
- Remaining material shader and particle-layer/cutoff families; broader model source compatibility.
- Remaining WXL client extensions, UI, WMO/ADT and related modules, following the same module interfaces.
- DX11 is a separate deferred task; DX11-PORT.md is historical dependency research, not a prerequisite for this x64 WXL build.

Compilation and source binding do not imply gameplay acceptance. Keep installed clients and rollback separate; no new Downloads folders or repeated gameplay probe loops are part of this work.

## Appearance/effect/protocol integration batch

The five native appearance controls, counts, nth lookups, randomization, validation and Init now use one retail choice authority when the native default/actual model path matches a mounted MD21 retail recipe. The five classic byte fields are limited to256 values per axis. Class/race/sex rebuild creates a candidate before retiring the previous actor; additional choice overrides survive same-family rebuilds.

WXL_GetRetailOptions/GetRetailChoices/GetRetailSelection and WXL_SetRetailChoice expose explicit option/choice IDs. WXL_RetailSelectionCanSave reports whether the selection fits the current native save path. Extra nondefault option IDs use the modular appearance-session owner: catalog negotiation, exact prepare/ACK matching, one-use native create dispatch and bounded stale/session/name/choice cancellation. Saved snapshots restore transactionally in character selection and on the active player. Alternate forms, synthetic profiles, barber state and nearby units remain outside this owner. No server ACK/save is simulated; authenticated persistence remains untested.

Recipe-selected skinned children use typed native model lifetime and complete same-index skeleton verification. They share source pose updates, rendering, lighting, scene migration and shadow traversal. Children with differing/subset skeletons or child emitters/lights/point attachments remain unsupported; appendage culling bounds are not expanded yet.

EXPT now supports finite constant RGB/alpha multipliers through the same private-key bake as EXP2, retaining source emitter order and native Screen behavior. Nonzero zSource, layered UV/shader rendering and animated cutoff remain unsupported.

Area-loot, garrison and spell-charge consumers now publish validated session-owned state through their existing WXL Lua APIs and WXL64 aliases. Spell-charge presentation installs after GameUI initialization, not during early Lua setup. See modules/protocol/CONSUMERS.md. Existing UI/data archives are not modified by this source batch.
