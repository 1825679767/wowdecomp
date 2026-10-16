# Actor-selected retail item body layers

Frozen source-only candidate based on native HEAD `6b1838b392fa722f0c07475e65fdecde407575ff`. Pending integration and combined build. No shared source writes, builds/tests, game/server/account actions, installs, packaging, publication, Downloads or DX11 work were performed.

## Scope and integration

Eight complete candidate files and `integration.patch` are supplied. `manifest.json` records exact independent source-before hashes/sizes, donor input, candidate hashes and patch hash. Compare each before hash before copying; merge only the corresponding hunks if it changed during the broad build. This stage depends on the already integrated ItemGraph and EquipmentAttachments module. No CMake, registration, feature bit, UI, wire protocol or PrivateAppearances edits are needed.

Files:

- `WXL/modules/db2/items/ItemDisplayIndex.hpp`: retain the exact resolved material texture path.
- `WXL/modules/db2/items/RetailItemIndex.cpp`: intern that path from actual ResolveTexture output and re-intern it when CloneCurrent copies immutable graph generations.
- `WXL/ItemGraph.hpp` and `WXL/modules/db2/items/NativeItemRows.cpp`: copy that path into the public owned material DTO.
- `WXL/EquipmentAttachments.hpp` and its module source: actor-selected component layer resolver.
- `WoW/Common/ComponentCore/CharacterComponent.cpp`: actual native AddItem, LoadItemComponentTexture and UpdateTextureSlot integration.
- `WXL/modules/appearances/AppearanceCompat.cpp`: wait for native cached item sections before atlas composition and include exact cache paths in the immutable atlas identity.

## Selection and authority

Only graph-owned missing native DBC displays use the new path. Complete native DBC rows retain their original native texture resolution and priority behavior. No global DBC row or server item cache is changed.

The resolver copies owned ItemGraphMaterials, bounded to 256 rows. It selects actual ItemDisplayInfoMaterialRes component rows: `layer` is the native ComponentSection 0..7, `textureType` and `modelIndex` are the donor missing sentinel. Replaceable model material rows are excluded. This is the donor's actual ComponentSection mapping, not a mapping guessed from a basename or a model's texture type.

Rank matches the existing donor equipment material policy: exact authoritative retail recipe race 6, verified mounted native family fallback 4, neutral 0, exact sex +1; incompatible race/sex is rejected. Equal rank retains the donor last-row-wins material rule. The resolver reuses the equipment module's mounted ChrRaces/client-prefix fallback, explicit allied-family table, private actor rules and form-specific body suppression. Race 28 Murloc and dragon recipe families 52/70 do not acquire humanoid body layers.

The exact original ResolveTexture path survives graph publication and DTO copying. No `Item\\TextureComponents` or `Item\\ObjectComponents` basename reconstruction, suffix guessing, first-variant fallback or global string alias is used. Each selected path must be mounted, a real BLP, and fit the native CACHEENTRY 128-byte filename field. An invalid/unresolved actor resource preserves the existing item presentation and records the existing pending-display retry instead of selecting an unrelated variant. Long paths intentionally fail closed rather than being truncated by native TextureCache.

## Classic compositor and retail atlas

The plain classic component can use this typed pre-compositor resolver. Native AddItem creates a stack-owned presentation copy whose eight component strings refer only to this dispatch's owned paths. Existing native s_itemPriority and UpdateItem methods remain responsible for item slot, section, ordering, glove/chest lower-arm precedence, robes, pants/boots overlap and exposed-foot behavior. Empty selected sections also invoke the native removal method so a previous variant cannot leave an obsolete overlay behind. The stack copy never escapes into the global row store or a deferred request.

The native section records retain display IDs as before. When ItemsLoaded later needs a texture, LoadItemComponentTexture resolves the actor's selected full path again and TextureCache takes its own filename/handle ownership. UpdateTextureSlot compares the actual cached filename for modern component rows, rather than the synthetic global row's intentionally blank or neutral string pointer. Identical cached images retain their handle while updating the native owning display ID.

AppearanceCompat now calls ItemsLoaded nonblocking before capturing its atlas signature. A pending native cache load is treated as transient, outside the failed immutable-recipe memoization path. The existing atlas Build then uses ReadImage and atlas::Paint on the same selected section handles in the same native priority order and actual retail layout section rectangles. There is no second armor overlay pass that could bypass native ordering or paint a layer twice.

The immutable atlas signature now includes each exact cached filename, encoded without hash collisions into its existing owned integer vector. An unchanged item display ID with a different actor-selected material therefore rebuilds the atlas. Guild/native cached section paths are represented by the same ownership rule. Native classic body construction continues through its original compositor; the new resolver supplies only the selected component texture source.

## Remaining gates

No build or tests were run here. The parent performs the combined compile after integration. Mounted material files, race/sex variants, neutral fallback ranking, shirt/chest/glove/tabard priorities, robe/pants/boots overlap, classic bodies, retail layouts, async graph/cache readiness, same-display variant changes and removal still require runtime visual acceptance.

Native CACHEENTRY paths of 128 bytes or more remain unsupported in this slice. ComponentSection values outside the donor's eight classic body sections and specialized per-batch material targeting are not inferred. The existing modern-only enchant metadata, heuristic collection retargeting and retail helmet policy gaps remain unchanged. This source candidate does not claim full retail equipment parity or accepted gameplay output.
