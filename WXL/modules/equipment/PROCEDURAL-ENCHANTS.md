# Native procedural enchant source stage — 2026-10-16

Frozen source-only candidate. No build, tests, game run, install, publication, or acceptance occurred. Parent integrates and performs the combined compile. HEAD observed while staging: a6e243bb257d00241c22aa48cf522c7326560964; source was deliberately dirty. manifest.json records ten exact source-before snapshots, the captured dirty diff, three behavior inputs, twelve candidate outputs, and the integration patch. Do not rerun stage.py against a later tree: it is the initial generator and does not include the final stable row-order adjustment.

## Implemented path

Modern ItemVisuals default kits and ItemVisualsXEffect weapon-subclass kit links resolve SpellVisualKitEffect EffectType 1 to an owned SpellProceduralEffect DTO. The DTO preserves kit/link/procedure identity, modern subtype, all four raw value bits, subclass and source-effect selector, and compatibility audit flag. The resolver uses the existing mounted filtered DB2 worker and publication generation; it adds no thread or borrowed row references. Exact PTR12.1 schema layout B442AB81 is used. At most 4096 requested procedure IDs, 65536 filtered rows, 256 procedures per kit/visual, and 1024 active item owners per scene are accepted. KitEffect row order is stable for repeated writes.

The explicit compatibility subset accepts inner Type 1 only with finite integral packed RGB Value[0] in 0..16777215, and inner Type 14 only with finite alpha Value[0] in 0..1. All three unused values must be zero. These shapes match the native WotLK CharProc1/14 and Cata Glue consumers. This is a deliberately bounded cross-generation compatibility assumption; it is not proof that every modern procedure subtype shares the legacy enum. Outer EffectType 1 selects the procedure table; it does not itself mean diffuse tint. There is no retail ID cast into a WotLK visual/kit/procedure ID or generic subtype dispatch.

ApplyItemVisual releases the previous procedural owner before replacing visual children. Only the modern fallback branch applies this policy, preserving the native DBC visual path. The actual item-parent CM2Model receives RGB diffuse tint and/or fade alpha. State retains the exact selected DTOs, baseline values and last writes. Reapply, feature disable, parent destruction and scene destruction restore owned values, while intervening native writers update the rollback baseline and are not overwritten during release. The owner is attached to the native model and intrusive scene list; the pre-worker scene animation pump checks its model handle and scene. No reference cycle, global actor lookup or root-character tint is introduced.

Default ON build option WXL_ENABLE_PROCEDURAL_ENCHANTS is compiled into the existing WxlEquipmentAttachments module. CVar wxlProceduralEnchants allows runtime rollback; disabling restores owners at the next native scene tick. It also follows the existing equipment/native/assets/DB2/BLP gates. Turning it back on takes effect on the next ordinary item-visual apply. Module API exposes procedural-enchants and its dependencies.

## Merge seams

Copy new WXL/ProceduralEnchants.hpp and WXL/modules/equipment/ProceduralEnchants.cpp. Merge the narrow shared hunks from integration.patch after checking manifest source-before hashes: ItemGraph DTO/vector, RetailSchemas definition, ItemVisualGraph procedure resolution/append/cleanup, EquipmentAttachments registration/release/apply, model state pointer/destructor, scene list/count/destructor/pre-worker tick, ModuleApi and existing equipment CMake target.

The timed kit-model worker owns overlapping ItemGraph/ItemVisualGraph/EquipmentAttachments/M2Model/M2Scene/CMake seams. Keep its timing DTOs, model owner pointer/list and lifecycle calls alongside these separate procedural fields and calls. Both scene pumps must run before animation workers; both model and scene owners must release before native teardown. Do not replace a newer whole shared file with this snapshot. Candidate full files normalize original line endings, while integration.patch contains only semantic hunks.

## Limits and prerequisites

Other outer effect types (including EffectType 6 -> SpellVisualAnim), other procedural subtypes, nonzero auxiliary values, timed alpha/fade flags, attachment positioners, animation kits, camera/audio/trail behavior remain counted unsupported. The raw DTO retains resolved incompatible procedure rows; missing tables/rows and cap overflow are counted unsupported. No guessed whole-model scale, guessed assets, fabricated enum relationship, global DBC mutation, native renderer replacement or server action is included. Model-attach timing is the other worker's stage.

Runtime still requires mounted matching SpellProceduralEffect/visual graph data and ordinary source integration. Source review/data inspection is not gameplay or visual acceptance. No outputs were built, and there is no build-source binding in this stage.

## Primary evidence

- Native WoW/Source/Object/ObjectClient/Unit_C.cpp CreateProceduralEffect, CharProc 1 and 14, and Cata reference WoW/Source/Glue/CGlueMgr.cpp ApplySpellVisualKit cases 1 and 14; exact file hashes are in manifest behaviorInputs.
- Mounted loose E:/RetailUnpacked_PTR_12.1.0/dbfilesclient/spellproceduraleffect.db2, WDC5 layout B442AB81, 11015 records, two physical fields; hashed as an inspected source input only, not copied or installed.
- Primary schema relationships: https://raw.githubusercontent.com/wowdev/WoWDBDefs/master/definitions/SpellVisualKitEffect.dbd
- Exact modern Type/Value[4] schema: https://raw.githubusercontent.com/wowdev/WoWDBDefs/master/definitions/SpellProceduralEffect.dbd
- Historical legacy CharProc fields: https://raw.githubusercontent.com/wowdev/WoWDBDefs/master/definitions/SpellVisualKit.dbd

All edits are inside this E-only stage. Original native source and runtime files were not modified by this worker.
