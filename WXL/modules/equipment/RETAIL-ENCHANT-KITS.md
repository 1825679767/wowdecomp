# Native enchant kit model subset

Source-only three-file follow-up against frozen HEAD 78b19e1385c100bb37d695a8bc5392187dd0d52d. No shared source edits, builds, tests, game launch, deployment or acceptance. Merge `native-retail-enchant-kit.patch` after the first enchant stage. Full candidate files and exact before snapshots are retained; preserve later root edits when merging.

Implemented graph and consumer:

- Gather the actual ItemVisuals SpellVisualKitId1/2 and ItemVisualsXEffect SpellVisualKitID values. They remain retail graph keys; no native WotLK kit/visual ID is called.
- Reuse existing identical RetailSchemas spell declarations and RetailSpellCatalog joins: ModelAttach parent kit; KitEffect EffectType 2 -> ModelAttach row ID; ModelAttach SpellVisualEffectNameID -> EffectName ModelFileDataID. Direct and indirect references deduplicate by kit/attachment-row pair, preserving shared attachments across multiple kits.
- One bounded dependency load per graph startup on the existing joined item worker. At most 4096 distinct dependency IDs per table query, 65536 decoded rows per selected table, 256 model children per visual. Optional kit failure preserves previously loaded direct ItemVisuals models and XEffect children. No new worker or CMake target.
- Owned C++17 DTOs retain kit/source-effect/attachment-row/model identities and selectors. Source default kits are unconditional; XEffect-derived model rows retain that row's exact weapon-subclass selector and source-effect ID. Native DBC visual records continue to win before this branch.
- The actual EquipmentAttachments consumer creates validated native CM2Model children, uses the ModelAttach attachment ID, multiplies ModelAttach and EffectName scales using the donor graph's nonpositive/nonfinite -> 1 fallback, preserves static Offset, explicit loop AnimID, and EffectName Alpha. Native item parent owns attached refs; RAII drops only creation refs. Replacement, unequip and parent release use the existing native child lifetime. No stock effect row or raw ID alias is synthesized.
- BuildSlot call is qualified as equipment_attachments::ApplyItemVisual, fixing the namespace error caught in root's combined compile of the first stage.

Exact supported subset and remaining limits:

- ModelAttach requires a real nonnegative attachment ID <=255, finite Offset, zero PositionerID/Flags/AnimKitID/StartDelay, zero Yaw/Pitch/Roll and their variations, zero OffsetVariation and ScaleVariation. StartAnimID/EndAnimID must be absent (0/FFFF/FFFFFFFF); explicit loop AnimID is retained. Native HasAttachment validates the point before creation.
- EffectName requires a direct model FDID, finite positive combined scale, finite Alpha in [0,1], and no TextureFileDataID, DissolveEffectID, Flags or ModelPosition substitution. Existing native asset conversion and shader/particle gates still apply. No gate was widened.
- Procedural KitEffect types other than 2, kit-only sounds/decals/area models, positioner motion, orientation/randomization, anim-kit/start/end timelines and texture/dissolve substitutions are not invented. Unsupported/missing kit rows increment owned unsupportedKitRows metadata. This is the donor model graph subset, not full retail SpellVisualKit playback.
- XEffect's separate direct AttachmentModelFileID retains its original consumer and outer scale. Kit-derived children use their own explicit ModelAttach attachment/scale data rather than guessing an additional XEffect transform or redirecting them into another model's skeleton. This does not claim retail procedural kit equivalence.

Source evidence:

- Existing WXL/modules/spell-visuals/RetailSpellCatalog.cpp: exact direct/indirect joins and model-scale multiplication; RetailSpellSchemas.hpp matches the item module's existing RetailSchemas.hpp declarations.
- Retained PTR DB2 headers match runtime definitions: SpellVisualKitEffect E3206CA2 (576599 total rows / 2 fields), SpellVisualKitModelAttach 02CF8554 (286500 / 22), SpellVisualEffectName 2245CEE6 (43865 /16). Runtime filters select only requested enchant dependency subsets. Loose inputs establish schema provenance, not installed MPQ identity.
- There is no generic modern enchant Kit record playback owner in the Win32 donor. This stage uses the concrete model relationships already implemented in its RetailSpellData graph, without treating procedural-only fields as native gameplay or particle policy.
