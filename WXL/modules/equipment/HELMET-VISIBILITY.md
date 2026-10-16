# Native modern helmet visibility

Frozen source-only candidate against HEAD `6b1838b392fa722f0c07475e65fdecde407575ff` **with the actual already-dirty working tree** containing integrated body-layer/material-fallback work. This is not a clean-HEAD snapshot. `manifest.json` records the exact six source-before files independently, their dirty diff from HEAD, donor inputs and all candidate hashes. No shared source writes, builds/tests, game/server/account actions, installation, packaging, publication, Downloads or DX11 work were performed.

## Integration

Six complete candidate files plus a unified patch are supplied. Compare every source-before hash before copying. Merge individual hunks if any of these files have since changed:

- `WXL/ItemGraph.hpp`: bounded owned helmet visibility DTO/API, including the original animation-scale metadata.
- `WXL/modules/db2/items/NativeItemRows.cpp`: typed graph lookup, sex visibility selector and copied rules/scale.
- `WXL/EquipmentAttachments.hpp` and module source: actor policy, rule consumer, component-owned mask restoration and loaded head-model gating.
- `WoW/Common/ComponentCore/CharacterComponent.cpp`: restore owned helmet geometry before native GeosRenderPrep; explicitly prevent graph visibility IDs from entering the legacy helmet visibility table.
- `WXL/modules/appearances/AppearanceCompat.cpp`: restore owned helmet geometry before recipe Bind, including after a nested native geoset pass, so recipe choices become the new base mask.

Existing equipment module/default-ON feature/CVar and all four post-compositor calls are reused. No build registration or new component pointer is required. Keep the root's already integrated body-layer changes in these full files; they are part of this stage's exact source inputs.

## Actual donor behavior ported

`RetailEquipment.cpp::HelmetActorIdentity`, `HelmetRuleMatches`, `ApplyRetailHelmetGeosets` and `RetailHelmetPolicy.hpp::RetailHelmetActor` define this slice. The existing graph's sex-specific ItemDisplayInfo HelmetGeosetVis chooses a visibility ID. Rules match an explicit retail model RaceID, or a non-race row whose RaceBitSelection includes both low bits (`&3 == 3`). Matching hide group N hides only section IDs N01..N99. ID 0 and N00 are preserved. Unsupported hide groups above 99 are ignored as in the donor. Rule flags are retained as metadata; the donor does not interpret them.

The consumer uses the actual retail recipe/model family and source-owned equipment state. Broken wire race 23, race 16, Eredar/Broken model paths, disguise actors and unknown family zero are excluded. Base races 1..11 require an actually modern-loaded model or an `_hd.m2` path. Optional `hd\\` prefixes are normalized before requiring a Character/Creature model root. Native DBC display rows are excluded from the graph API and keep their original native helmet model and visibility behavior. Broken retains the existing private profile/native helmet remask unchanged.

The mask is applied only when the equipped graph head slot has completed its actual model build and owns at least one loaded child. Missing, model-less, unresolved or unremappable helmets cannot hide the character's hair/ears. Model selection and paths remain under the existing graph FDID/model-family consumer; no generated helmet asset alias or geometric retarget heuristic is added.

## Per-model ownership and restoration

Visibility changes remain in each native CM2Model instance; no global DBC or shared SKIN data is mutated. The existing component-owned equipment state carries the model pointer/handle, component serial, head display/generation, copied bounded helmet data, skin profile identity and exact per-section visibility values that this consumer changed from visible to hidden. At most 256 metadata rules and 16384 skin sections are admitted. Ordinary post calls do not repeatedly show/hide unchanged sections.

BeginGeometry restores only this owner's captured sections while the component/model/skin identities still match, then clears its snapshot before the native component or retail recipe writes a new baseline. The recipe seam also clears a mask applied by its nested native geoset pass. This prevents restoring stale hair/ear choices over newer customization or body geometry. Restoration preserves the original per-section native values, including duplicate section IDs, and invalidates/rebuilds native optimized geometry through its typed methods.

Head invalidation, unequip, replacement, CVar disable and release restore the owned mask. Actor retirement already releases equipment state before native model release. Native RemoveItem and recipe rebuilds continue to own their ordinary component changes. No saved borrowed graph pointer, pointer-keyed global actor map, global alias or renderer hook is used.

## Animation-scale metadata and remaining limits

The donor graph loads and exposes HelmetAnimScaling, but an inspected source search finds no runtime consumer: its only modern-m2 use copies Amount into HelmetData. Native CM2Model exposes world-transform scale, not a proven helmet-animation scaling operation. Following the root's explicit decision, the new DTO retains the exact optional `(visibility ID, retail model race)` Amount and marks its presence, but does not apply it. Whole-actor/whole-helmet scale would be an invented behavior, not a missing donor port.

The donor loads six ItemDisplayInfo GeosetGroup fields but its actual character geometry consumer maps only robe index 2 and cape index 0, already represented by the native component path. No proven runtime mapping for additional indices 3..5 was found, so this stage does not guess one. Specialized generated retail helmet assets, heuristic retargeting, or propagation of parent hide rules into separate customization-child geometry remain outside this slice.

No build or tests were run. Parent combined compilation must validate the source integration. Actual mounted helmet/rule data, male/female visibility selection, non-race bit rules, customization under an equipped helmet, model-less head records, re-equip/unequip, native DBC and Broken preservation, CVar disable, recipe rebuilds and actor retirement remain runtime acceptance gates. This is implementation evidence, not accepted visual/gameplay parity.
