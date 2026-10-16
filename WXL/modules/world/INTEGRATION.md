# Native WMO and split ADT source modules

This source-only stage adds two native byte/provider modules. No tracked checkout
edits, Data copies, builds, tests, native gameplay, installation or acceptance
occurred here. No renderer, fog, day/night or liquid rendering code is replaced.

Root integration:

- Add default-ON `WmoEnabled()` and `SplitAdtEnabled()` feature accessors, following
  the existing master native flag and respective runtime feature gates.
- Compile `WXL/modules/world/WmoNative.cpp`, `SplitAdtNative.cpp`, and the four
  vendored WMO implementation files `WmoAssetFamily.cpp`, `WmoRootBytes.cpp`,
  `WmoMaterialPlan.cpp`, `WmoChunkIndex.cpp` as C++20 native objects. Existing
  include roots for repository, WXL, vendor/src and vendor/include suffice.
- Link the existing native storage, FDID, runtime log and DB client modules.
- Apply the three staged WorldClient source seam changes, preferably by diff
  rather than replacing a file changed concurrently.

`MapObjRead.cpp`: normalize a complete modern root family after its original
async object has been retired and before the stock positional root parser runs.
All group bytes must resolve, validate and normalize before publishing root
replacement bytes. `ReadGroup` checks the root-owned indexed group provider
before its original named-file path. `MapObj.cpp::Clear` retires provider state.

Classical roots stay on the original native path. Modern roots use the donor's
real GFID/MODI/material parsers and existing shader compatibility families.
Group paths come only from GFIDs through the mounted FDID authority or the exact
verified source `ReadGroup` filename contract. No storage alias, binary host,
fixed address or guessed WDT route is installed. Doodad definitions and all
resolved asset names remain owned. Material lookup and shader fallback reuse
existing WXL behavior; layered shader 23 remains refused rather than flattened.

The module rewrites groups into the exact native mandatory/flag-ordered optional
chunk layout, checks geometry/material references, and preserves native BSP,
lights, doodads, colors, a second UV/color stream and supported native liquids.
Extended validated batch material indices are reduced to the native byte field.
32-bit geometry, layer weights, unsupported replacement batches, triangle strips,
unrecognized metadata and unsupported group layouts remain refused. Root count
is capped at the native object's 512 group slots. Prepared providers are bounded
to 128 roots, 128 MiB per family and 256 MiB globally. Group buffers are copied
into SMem-owned native allocations; stock group creation/destruction owns them.
Root replacement likewise uses the native SMem allocator. Synchronous provider
loading is a functionality/performance gate requiring later runtime acceptance.

`MapArea.cpp::Load` detects the actual caller's `_tex0.adt` and `_obj0.adt`
companions. No companions means the original monolithic async path. A complete
trio passes through the donor's real texture/object/header adapters. Alpha policy
comes directly from `CMap::header.m_flags` (the consumed MPHD); no WDT filename
is guessed or opened. Missing members or failed preparation publish nothing.

The native module then assembles one owned native ADT buffer: fresh MHDR/MCIN,
resolved MTEX/MMDX/MMID/MWMO/MWID, original placement arrays, root geometry,
merged texture data and concatenated MCRD/MCRW references. MCNK native counts,
sizes, coordinates and offsets are bounded. Native first-use MCNR spans are
padded to their 448-byte convention. The stock CMapArea/CMapChunk parsers and
all subsequent world rendering remain responsible for the result. Allocation
uses `AllocAsyncLoadBuffer`; the existing area destructor frees that exact buffer.

Unresolved/dropped object placements are refused. Native-compatible MH2O data
is preserved after donor-format bounds validation and live LiquidType lookup.
Unsupported placement-object liquid formats are refused rather than guessed or
silently remapped. Active sound emitters and active legacy MCLQ rewriting remain
outside this subset. High-resolution holes use the donor's native compatibility
collapse. Extra height/baked-light metadata is retained as opaque bytes, while
native stock rendering still owns visual behavior; modern height blending is
not supplied by this stage.

The staged source seams fail closed on recognized split/modern families that
cannot be transformed safely, using the native asset-failure error mechanism.
Sending a missing-MOTX FDID WMO root or an incomplete split ADT through the stock
positional parser would be unsafe. Unsupported *native-readable untransformed*
inputs retain the original path; malformed or incompatible replacement data is
never published as a valid transformed asset. Root may adapt the visible native
asset-failure presentation, but must preserve that parse boundary.

Donor source provenance is `src/platform/waystone` in the x64-waystone worktree,
plus `engine/assets/shared/models/wmo/ShaderFallback.hpp`. Copyright notices are
retained. The vendored `WmoLayout.hpp` deliberately retains disk chunk slots only;
all executable RVAs, native layout offsets, signatures and probe declarations
were removed. `AdtPreparedByteFamily.hpp` adds only unresolved/suppressed placement
refusal to the copied transactional producer. Other donor parsers are unchanged.

Source compilation, real mounted-family fixtures, runtime lifetime/streaming,
native liquid/fog parity, visuals and gameplay remain separate validation gates.


Root applied the staged native seams and added default-ON WXL_ENABLE_WMO /
WXL_ENABLE_SPLIT_ADT with wxlModernWmo / wxlSplitAdt CVars and module descriptors.
Group provider results distinguish Classic/Loaded/Rejected; an admitted group
allocation or identity failure never falls through to incompatible named data.
The native split adapter explicitly refuses an existing tex1 alternative.
Synchronous source preparation is integrated functionality, not an async
streaming performance claim. Broad runtime testing remains deferred by request.
