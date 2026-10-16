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
| client-extensions | WxlClientExtensions / WXL::ClientExtensions | native helpers, legacy addon packet codec/dispatch and paragon owner |
| world | WxlWorld / WXL::World | modern WMO root/group providers and split ADT native byte families |
| appearance-session | WxlAppearanceSession / WXL::AppearanceSession | hello/prepare/create and restored player choices |
| protocol | WxlProtocol / WXL::Protocol | bounded custom packet transport, session lifecycle and Lua APIs |

Public headers remain under WXL/. Native call sites use those headers; implementations live under WXL/modules/. DB2 no longer depends on a model adapter just to read files, and BLP preprocessing is separate from model loading. Narrow native Gx, Model2, character and network seams stay with their native owners.

## Feature controls

All implemented features default ON. CMake options: WXL_ENABLE_NATIVE (master), WXL_ENABLE_ASSETS, WXL_ENABLE_DB2, WXL_ENABLE_BLP, WXL_ENABLE_MATERIALS, WXL_ENABLE_PARTICLES, WXL_ENABLE_APPEARANCES, WXL_ENABLE_PROTOCOL, WXL_ENABLE_APPEARANCE_SAVE, WXL_ENABLE_WMO, WXL_ENABLE_SPLIT_ADT, WXL_ENABLE_CLIENT_EXTENSIONS. Options control feature availability; interface implementations remain linked so native call sites have defined disabled behavior.

Runtime CVars: wxlModernAssets, wxlRetailDb2, wxlModernBlp, wxlCustomMaterials, wxlParticles, wxlRetailAppearances, wxlCustomOpcodes, wxlAppearanceSave, wxlModernWmo, wxlSplitAdt, wxlClientExtensions. Asset/DB changes require restart; model changes require reload. Appearances additionally require assets, DB2 and BLP; material/particle adaptation requires assets. Disabling dependencies suppresses the dependent path without changing its saved setting.

C++ ModuleCount/QueryModule expose version1 module descriptors. Lua WXL_GetNativeModules() returns id, apiVersion, dependencies and effective enabled state. NativeProtocol.md documents binary custom packet send/pop APIs; transport readiness is not a server acknowledgement or complete feature parser.

## Integrated native paths

CM2Shared converts modern data before native relocation and owns converted SKIN companions until destruction. Per-section palettes use the actual backend constant budget. Retail high-word index offsets are consumed throughout upload/render/intersection; the legacy uint16-count optimizer is bypassed for modern topology over65535 indices rather than truncating it.

Sized sync/async BLP sources keep transcode buffers alive through upload. The appearance module joins mounted retail customization/material/layout tables, checks exact model identity, composes private atlas sheets and equipment layers, applies geosets and owns replaceable textures with native teardown. Creation preferences are sized from mounted race IDs instead of the former22-race array.

Mesh blend7 ONE/INV_SRC_ALPHA remains separate from particle Screen INV_DEST_COLOR/ONE. EXP2 constant RGB/alpha multipliers are baked into private native keys. EXPT supports emitter RGB/alpha constants with neutral TXAC. EXP2 takes precedence when both extension chunks occur, matching source semantics. Identically-zero cutoff tracks support the full uint16 lifetime domain; unsupported nonzero cutoff, zSource and layered rendering remain refused.

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

Recipe-selected skinned children use typed native model lifetime and a verified child-to-source skeleton map. Ancestor-closed subsets and reordered bones are supported when exact identifiers, bind pivots and parent topology establish a unique injective correspondence. They share source pose updates, rendering, lighting, scene migration and shadow traversal. Ambiguous skeletons, omitted ancestors, differing bind pivots or child emitters/lights/point attachments remain unsupported; appendage culling bounds are not expanded yet.

EXPT now supports finite constant RGB/alpha multipliers through the same private-key bake as EXP2, retaining source emitter order and native Screen behavior. Nonzero zSource, layered UV/shader rendering and animated cutoff remain unsupported.

Area-loot, garrison and spell-charge consumers now publish validated session-owned state through their existing WXL Lua APIs and WXL64 aliases. Spell-charge presentation installs after GameUI initialization, not during early Lua setup. See modules/protocol/CONSUMERS.md. Existing UI/data archives are not modified by this source batch.

## Broader native integration batch

Creation now has a paginated, template-independent retail-detail panel for explicit extra options. The native randomize command includes extra options, and the panel refreshes from actual component/lifetime/preferences/revision changes. The five stock axes remain in their native controls. UI/archive layouts can be refined after broad testing; no shared or locale archive is changed.

Source-native player UpdateDisplay restores server appearance snapshots for nearby players as well as the active player; actual default model checks keep NPC disguises and transformed families out. Queries share a four-per-second budget, three attempts per refresh, bounded512 actor caches, sixty-second revision refresh and idle eviction. Active barber edits suspend restoration. This does not add an unsupported barber save opcode or authenticate server persistence.

WXL::World integrates modern WMO root/material/FDID normalization and root-owned group providers before the stock parser. A complete prepared family publishes atomically; native group creation and SMem lifetime own copies. Provider failure is distinct from classic fallback. Split ADT loads the real caller's exact root/tex0/obj0 family using consumed native MPHD alpha policy, shapes an owned monolithic native buffer and retains stock terrain parsing/rendering. tex1 alternatives, unresolved placements, unsupported liquid layouts, 32-bit WMO geometry and incompatible layered shaders remain refused. There is no guessed WDT/MAID route or global alpha mutation. This adds source loader support, with synchronous preparation and runtime streaming/visual acceptance still pending. Native world/fog/liquid/weather rendering stays native. See modules/world/INTEGRATION.md.

Client extensions now supply CreateWXLPacket/OnWXLPacket, typed bounded little-endian codecs, source-whitelisted raw custom IDs, private native-consumer exclusion, session-safe readers/writers and an owner-side callback pump. Logging, spell descriptions, Unicode command-line arguments and mounted text reads retain the donor APIs. Paragon has its own bounded compiled request/state owner and original Lua entry points. Native gameplay handlers for other raw addon IDs are not implied by the facade. See modules/client-extensions/INTEGRATION.md.

## Alternate appearances, cutoff rendering and radial pings

Primary/alternate creation actors now have independent model-family authorities and explicit option states for wire12 (22/23),17 (76/70),30 (75/52). The canonical native actor stays primary; form preview changes scene attachment/editor target. Capture requires both current independently bound actors, stamps both lifetimes/handles/revisions and sends both canonical arrays. Snapshot restore validates/allocates both states before publication. Actual alternate choices survive actor rebuilds through bounded session/GUID/race/sex retention. Worgen Human supplemental Eye Color follows the existing exact-FDID donor rule. Alternate body garments are copied; family-specific helm/shoulder/cape/weapon attachments and synthetic profiles remain separate. See modules/appearances/ALTERNATE-FORMS.md.

Nonzero EXP2 single-texture sprite cutoff now retains source-owned UNORM16 lifetime tracks and samples each rendered particle. Ordered native index slices use the existing floating alpha-reference Particle shader permutation, preserving native Gx blend/lighting/fog and restoring alpha state. Runtime stock shader Valid() is required; no new shader bytecode/compiler or DX11 path is supplied. Layered/model particles, zSource and fixed-function cutoff remain outside this path. Per-particle slices can cost extra draws; broad performance/visual testing is still pending. See modules/particles/CUTOFF.md.

WXL::RadialPing replaces generic raw handling of521/522 with an exclusive compiled owner, native world picking/projection/unit positions and the existing C_Ping addon bootstrap. Packet/state queues are bounded and invalidated on session/map changes; UI callbacks run after capture locks. Default WXL_ENABLE_RADIAL_PING/wxlRadialPing is ON and follows native/protocol/client-extension gates. Quest-marker geometry, challenges and skyriding native handlers remain separate. Native companion sorting also handles pending/missing names with deterministic numeric fallback instead of unrelated-pointer subtraction.

A stable pending test-client folder is prepared separately under E:/WXL1.1/clients/WXL-x64. Its Data/Interface directory links share the existing Eunoia authority; its WTF/log/cache state and source-built executable family are separate. No Win32 proxy/DLLs are copied. DX11 remains deferred. Compilation, local staging and broad gameplay acceptance stay distinct.
