# Native retail item graph integration stage

Source-only candidate against the exact source-head.txt and before/ snapshot. No shared source edits, compile, test, game launch, installation or publication performed.

## Implemented

- Pure donor RetailSchemas, relationship joins, filtered display/model/material graph, immutable catalog/index publication. Donor resolver calls now use native FDID ownership/storage. Source provenance: E:/WXL1.1/extensions/wxl-db2/src/api/{RetailItemIndex.cpp,RetailSchemas.hpp,RetailItemCatalog.*,ItemDisplayIndex.*}.
- A single owned worker starts after ClientDBInitialize and is stopped/joined at the first WowClientDestroy operation and before CloseAllArchives closes any archive. No detached thread, Sleep loop, raw actor access, offsets or x86 row casts. Stop cancels waits; join waits for an active archive read to finish before shutdown proceeds.
- Bounded 4096 queued IDs, 256 filtered IDs per pass, 65536 total queued/inflight/resolved/abandoned display IDs; four attempts per failed batch; three startup attempts. Stopped/abandoned requests do not spin. Full relationship tables still use the existing native WDC5 decoder's file/row bounds.
- Typed WowClientDB<ItemRec> and WowClientDB_RLE<ItemDisplayInfoRec> only synthesize missing rows. Every existing native DBC row wins. Requested fallback rows and their strings have stable process lifetime, capped at 65536 rows per table. Native display records contain only native-owned weapon/offhand/ranged model strings; modern armor models remain owned by the separate equipment consumer.
- VisibleItem accessors preserve the existing server stat-cache priority, then use typed native/retail Item rows when no stats are available. Root corrected the staged early-row return to avoid overriding server/custom item presentation. Signed item keys use int64 magnitude and reject INT_MIN instead of abs overflow.
- Catalog/display publication advances a generation. ClientIdle snapshots up to 4096 visible player GUIDs and re-resolves up to eight players per tick before native EquipVisibleItem replay; no retained actor pointers. Generation changes coalesce while a replay drains. Hand visual slots use the native visual refresh seam. This is generation-triggered visible actor work, not repeated DB graph scans.
- Components own twelve pending display IDs/visuals. Missing AddItem requests replay on RenderPrep after publication; typed AddItem and RemoveItem clear pending state, including early RemoveItem paths. No borrowed component pointer registry.
- Public C++17 ItemGraph.hpp exports copied, bounded (256 entries) model/material DTOs, raw donor race/sex/model-slot/attachment/geoset selectors and inventory type. DBC-owned IDs are refused by this private modern presentation API. Private WxlDB2 implementation remains C++20.

## Integration

Apply integration.patch from the native repository root, or copy the new WXL/ItemGraph.hpp and WXL/modules/db2/items files. Merge existing files as hunks: WXL/CMakeLists.txt only adds four item sources; Client.cpp only adds lifecycle/pump calls and include; CharacterComponent.cpp/.h only adds pending arrays, retry and AddItem/RemoveItem cancellation. Parent has newer Broken/tail/Blood changes: never overwrite those existing files with frozen complete candidates. WowClientDB.h and VisibleItem_C.cpp are otherwise isolated. Full review/ files are supplied for audit.

## Concrete pending coverage

- Modern ItemVisual/ItemVisualEffects and ParticleColor graph conversion are absent in this donor graph. Synthetic rows deliberately leave those two IDs zero instead of treating retail integer IDs as legacy native table IDs. Existing DBC enchant presentation is unchanged.
- Global display rows cannot choose actor race/sex. Native fallback body layers with any race/sex-specific candidates are empty; their complete owned material selectors are exported for the equipment consumer. Neutral body layers and native geosets remain mapped. Form-specific Murloc/dragon suppression belongs to that per-component consumer.
- Extra modern geoset groups 4-6 and HelmetGeosetData/HelmetAnimScaling semantics are preserved in the private immutable graph, but the native three-group row alone does not consume those extensions.
- More than 256 models/materials per queried display fails closed. Exhausted request/row caps keep native/stat-cache presentation. No runtime or visual acceptance is claimed.
