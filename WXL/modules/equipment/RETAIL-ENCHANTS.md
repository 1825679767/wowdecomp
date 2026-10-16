# Native modern enchant model integration

Source-only overlay. No build, tests, game launch, install, packaging, or visual acceptance occurred. `files/` contains 14 complete candidate source files; `before/` and `manifest.json` bind each changed file independently. Root was concurrently advancing from requested commit 6b8280048f3a88af104b5b704e96a57aa1d76365 to d316528f40ae27893d3b3eb30d67102cc074bc8f. Apply patch hunks, never replace current shared files wholesale. Preserve root's independent BuildSlot SetSkinningSource collection/geometryScaleOverride edit (the before snapshot predates that line).

Implemented behavior:

- Optional exact-layout ItemVisuals 4D7EF22C and ItemVisualsXEffect 62F4C094 declarations. Retained loose PTR headers match: ItemVisuals 187 rows, 3 physical fields; ItemVisualsXEffect 3094 rows, 5 physical fields. Loose files establish schema evidence, not installed archive identity. Runtime reads mounted archive authority through existing DB2 adapter.
- Immutable owned visual catalog loaded on the existing joined item worker. No new thread. FDID -> exact mounted model path resolution stays in native FdidCompat. Stop joins before clearing catalog and before archive close. Public C++17 DTOs copy strings, arrays, and source kit/selector metadata.
- ItemVisuals direct five ModelFileID slots create real CM2Model children at native attachment IDs 0..4. Native ItemVisuals DBC record always takes precedence for explicit enchant IDs, including ID collisions; its ItemVisualEffects strings retain their existing consumer. Modern synthetic ItemDisplayInfo still writes itemVisual=0 rather than exposing modern integers as legacy effect IDs. Its intrinsic visual goes through an explicit display graph query.
- XEffect direct AttachmentModelFileID children use actual AttachmentID and positive finite Scale, selected by exact DisplayWeaponSubclassID equality with known item weapon subclass. Player enchant replay supplies authoritative VisibleItem class/subtype/display. For display-only consumers, an immutable catalog map admits only a unique weapon class/subclass across all known default/modified appearances; conflicting/nonweapon identity suppresses subclass-specific children. No wildcard or legacy enchant-ID remapping is invented.
- AddLink, AddAmmo, unit item-model creation, modular attached equipment, and player enchant replay consume the graph. Clearing an explicit enchant can restore the modern intrinsic visual because replay retains display identity. Native item parent retains child refs; scoped creation refs are released even when validation throws. Existing replace/unequip/model destruction owns child release and emitter/ribbon disabling.

Remaining boundaries:

- SpellVisualKitId1/2 and XEffect SpellVisualKitID remain owned metadata. This donor contains no generic modern enchant kit playback owner; no modern kit ID is interpreted as native WotLK SpellVisual ID. Direct attachment models from a row remain usable independently. Kit-only rows are therefore not rendered by this stage. Rift447445's bounded kit graph is not a generic equipment-kit implementation.
- Unknown/ambiguous weapon subclasses are not a wildcard. No guessed sentinel selector, class remapping, or effect-ID alias is accepted.
- Models still require existing native modern-M2/material/particle compatibility and exact mounted paths. An unsupported model is skipped; this stage grants no asset gate exceptions.
- Data loads occur at normal item-graph startup; missing optional visual tables do not break native DBC equipment. No detached worker, GPU resource, custom shader, protocol ownership, DX11, world/fog/liquid ownership, or AssetCompat/M2Bytes change.

Schema authority and donor distinction:

- https://raw.githubusercontent.com/wowdev/WoWDBDefs/master/definitions/ItemVisuals.dbd — modern physical fields: two kit IDs then five direct model FDIDs.
- https://raw.githubusercontent.com/wowdev/WoWDBDefs/master/definitions/ItemVisualsXEffect.dbd — explicit model/attachment/selector/scale plus noninline parent ItemVisuals relation.
- https://raw.githubusercontent.com/wowdev/WoWDBDefs/master/definitions/ItemVisualEffects.dbd — legacy model-string table, no modern 12.1 layout.
- Actual Win32 donor RetailSchemas/Index only retain raw ItemDisplayInfo.ItemVisual. RetailItem::WriteItemDisplay copies that raw integer into legacy m_itemVisual; it does not decode the modern tables. This overlay implements the table-backed native model portion instead of reproducing the unsafe alias.

Integration seams: WxlDb2 adds ItemVisualGraph.cpp; RetailItemIndex worker loads optional catalog after base relationship attempts and clears after join. CMake, EquipmentAttachments, Player_C, Unit_C, CharacterComponent, and ItemGraph hunks may overlap root work; merge individually. `source-dirty-at-freeze.patch` is provenance only, not this overlay's integration patch.
