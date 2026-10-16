# Typed native equipment attachment consumer

Source-only candidate based on native HEAD `89d5e1b9b9ea674239d186007fa0f4a65332471b`. Pending integration and combined build; unbuilt, uninstalled, no gameplay or visual acceptance. No shared source writes, builds/tests, server/account actions, Downloads, DX11 work, installation, packaging, or publication were performed.

## Integrate with the graph stage

This subdirectory owns its two new files and five shared wiring files. Its `manifest.json` captures exact source-before hashes, donor inputs, the graph public header dependency hash, all full candidate files, and the unified patch. Check each source-before hash before copying an unchanged file; otherwise merge its patch hunks. Preserve the graph worker's simultaneous `CharacterComponent.cpp/.h` and CMake changes.

New owned files:

- `WXL/EquipmentAttachments.hpp`: C++17 typed entry points and component state forward declaration.
- `WXL/modules/equipment/EquipmentAttachments.cpp`: C++20 module implementation using graph-owned DTO copies and actual `CCharacterComponent`, `CM2Model`, `CM2Scene`, mounted texture, and native DB owners.

Shared wiring:

- `CharacterComponent.h`: one initialized component-owned state pointer.
- `CharacterComponent.cpp`: call Apply after the existing private post-compositor guard at all four render/geoset/finished-request seams; release before destructor/Init retires the actor; invalidate owned slot models before RemoveItem; native AddItem form policy; native weapon/ammo material binding; validated native item visual consumer.
- `WXL/CMakeLists.txt`: default-ON `WXL_ENABLE_EQUIPMENT_ATTACHMENTS`, independent object module, and static facade collection.
- Runtime `NativeFeatures.cpp` and `ModuleApi.cpp`: register `wxlEquipmentAttachments` and expose the module/dependencies.

Apply the graph stage first or merge the component/CMake hunks together. This consumer requires exact `ItemGraphModels`, `ItemGraphMaterials`, `ItemGraphInventoryType`, `ItemGraphOwnsDisplay`, and `ItemGraphGeneration` APIs in the graph worker's `review/WXL/ItemGraph.hpp`. Graph queries enqueue unresolved displays and return copied data; the consumer never retains a graph pointer or aliases a global DBC row.

## Behavior implemented

Native DBC presentation wins. The module allocates no attachment state for components with only native DBC armor. Synthetic native graph rows retain the original split: armor object names remain empty, while weapon/offhand/ranged models remain owned by native hand-item code. This module creates armor models only; it never duplicates native weapon models or removes arbitrary native attachment-point children. Glove points 1/2 therefore cannot erase native weapons.

Each armor slot selects authored graph entries by exact modelSlot, attachId, and modelIndex. Race rank is exact retail family 6, verified native/client-prefix fallback 4, neutral 0, with exact sex adding 1 and incompatible sex excluded. Equal-rank model names use the donor lexical tie rule. Fallback identity uses the mounted native equipment race's actual ChrRaces prefix, plus the donor explicit allied-family mappings. Dragon forms use authored male equipment selectors. All graph DTO vectors are bounded by the producer to 256 entries and checked again here.

Object paths retain the donor `Item\\ObjectComponents\\<folder>\\<model>` contract and canonicalize MDX to M2. Missing mounted files, loaded fallback names, failed textures, invalid replaceable slots, and absent attachment points fail the slot build. Textures bind through actual model ReplaceTexture and native resource references; the owning model retains each binding. Replaceable material types 1..31 use the donor model-index and race/sex ranking. Component body-layer rows with no replaceable type remain under native compositor ownership.

Ordinary armor attaches through native HasAttachment/AttachToParent. Authored root collection attachId 19 uses native SetSkinningSource, requiring the existing exact native skeleton correspondence. It never fabricates attachment offsets, bone palettes, or a renderer replacement. Child particle/ribbon permission, visibility, transform and dynamic bounds are inherited by that source-native skinning owner.

Collection geometry also ports the actual RetailSkinProvider inventory families: helm 27; shoulders 26; shirt 10; chest 22/28; waist 18; legs 11/13; boots 5/20; wrists 8; hands 4/23; cape 15; tabard 9; robe 22/13. This filters native per-instance visibility without rewriting shared SKIN bytes. Explicit graph geoset filters intersect that inventory policy. A model with no surviving authored sections fails closed. Shared collection ownership uses model path plus inventory filter identity, matching the donor generated variant boundary, with deterministic lower-slot ownership for duplicates. Slot changes wake any later suppressed owner.

Murloc wire race 28 has weapon-only gear. Dragon recipe families 52/70 permit shoulders/waist attachment geometry only. Native AddItem clears the old native presentation and retains the new display identity while skipping humanoid body-layer/model application for those forms; the consumer then builds only allowed authored attachments. No server inventory or equipped item field is changed.

Native hand-item/ammo models get neutral graph replaceable materials only after matching their exact loaded model stem to a native-owned graph entry. Their native sheath/hand selection and enchant identity remain unchanged. The static native item path has no component race/sex owner, so it intentionally does not guess race/sex-specific weapon material variants.

The native ItemVisuals/ItemVisualEffects path retains its existing five attachment slots and original caller-selected visual ID. It stops old emitter/ribbon children before detaching, requires real native attachment points, checks mounted effect models and exact loaded names, then lets native scene attachment retain the validated child. Invalid/missing effects are skipped instead of displaying fallback geometry.

## Lifecycle and async ownership

Every attachment state belongs to its native component and records component serial, model pointer/handle, actor race family/fallback/sex, and display identity per slot. Release occurs before the native actor retires in both destructor and Init. Removing/replacing gear invalidates its owned children before native RemoveItem can detach those points. Destruction stops emitters/ribbons, clears skinning source or detaches only that owned model, then releases its reference. No pointer-keyed global actor registry is introduced.

Native component render callbacks perform at most two slot attempts per 16ms per actor, retrying unresolved resources after 500ms. Each slot is built as a complete candidate; failed models/textures/skeleton maps cannot publish a partial replacement. Lost native attachment linkage is detected from the retained typed model and triggers a bounded retry. Ready slots do not rebuild every frame or on unrelated global graph publications.

## Explicit remaining gaps

The graph worker confirms that this donor graph has no modern ItemVisuals/ItemVisualEffects schema. Modern-only enchant IDs and synthetic display particle-color/item-visual metadata remain unsupported; no fallback enchant packet, fabricated visual, or reused unrelated DBC row is created. Native DBC effects are supported by the typed consumer above.

Root collections requiring the donor heuristic retarget/geometry scaling instead of the existing exact native skeleton map remain unsupported. Native SetSkinningSource still refuses child cameras/attachments that it cannot safely inherit. Donor special helmet asset generation, retail helmet-geoset DB2 hide rules/animation scaling, actor-specific native weapon material variants, and special collection batch-target/material policies beyond donor replaceable-type selection remain outside this slice. Raw donor modelFlags/textureFlags are not cast into unrelated native flag enums.

No combined compilation or runtime verification was performed here. Parent integration/build must check source-level API compatibility. Actual archive availability, armor material appearance, native weapons with retail gloves, robe/leg collection boundaries, form changes, async resolution/re-equip, child emitters/ribbons/lights, native enchant placement, removal, CVar disable, and actor retirement remain runtime acceptance gates. Source completeness for this consumer is not a claim of full retail equipment or enchant parity.
