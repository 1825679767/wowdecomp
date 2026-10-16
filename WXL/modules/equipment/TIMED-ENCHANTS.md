# Native timed enchant model owner

Source-only follow-up against fe93326bcec7dfe242a638b1ce681b61db0e8613. No shared-source writes, build, tests, game launch, deployment or acceptance. Apply individual patch hunks; full files are review candidates, not permission to overwrite root's concurrent procedural/material changes.

Implemented:

- Owned kit DTOs now retain source StartDelay float, signed StartAnimID/EndAnimID, loop AnimID, Flags, Yaw/Pitch/Roll, Offset and scale. Source flags must still be zero; source PositionerID/AnimKitID/random variation stay refused. No modern kit ID is sent to a WotLK kit function.
- A per-parent CM2Model timeline owns native creation refs for kit-derived models. Its per-scene intrusive list has no process-global model pointer registry. Delayed entries create no model until due. Start phase advances to loop using actual native GetBoneSequenceInfo end/repeat timing; loop is replayed through the typed sequence API; replacement can play source EndAnimID before final release. Missing requested source animations refuse that child instead of selecting an invented animation.
- Scene Animate pumps owners before BeginThread/AnimateThread. It snapshots retained native parent refs before mutations, so recursive child teardown cannot invalidate a saved next-owner pointer. No Lua clock, detached timer, animation-worker mutation, asynchronous render callback, or external GUID lookup is needed.
- Same source visual/display/subclass and visual-catalog generation do not restart the phase on repeated player replay. Replacement advances owner epoch and cancels pending old entries. Old ending children remain owned while new children start. Child count is bounded at512 per parent, including end phases.
- Parent detachment/destruction and scene destruction release states. The pump checks scene, parent native handle, and current live attachment/skinning root pointer+handle; it never dereferences a stale captured root. Visual-catalog generation changes rebuild the source graph; unrelated item-display publications do not restart enchants. Retired identities are invalidated, so a later catalog publication cannot resurrect an explicitly removed old enchant.
- Every released owned child disables emitters/ribbons, detaches only from its expected item parent, and releases the held creation ref. A child adopted by another native parent is not detached or disabled by this owner. Existing native DBC visuals and direct five-slot/XEffect children keep their precedence/consumer.

Recovered static transform semantics:

`WoW/Source/Object/ObjectClient/Effect_C.cpp`, CEffect::ApplyModelAttachRecToMatrix, is the exact native model-attach implementation: rotate Z by negative yaw, then Y by pitch, then X by roll; at facing0 translate by (OffsetX,-OffsetY,OffsetZ). The timeline calls this typed existing helper with a stack SpellVisualKitModelAttachRec containing only the transform fields and an initial scale matrix, then calls native SetWorldTransform. No ID or table pointer is remapped. This corrects the prior model-subset consumer's positive Y offset assumption.

Explicit compatibility policy and rollback:

- Primary WoWDBDefs confirms StartDelay is float and animation IDs reference AnimationData, but neither retained Win32 donor graph nor inspected reference source establishes delay units. Root explicitly authorized a seconds compatibility policy rather than leaving positive delays unimplemented. This stage converts finite nonnegative source seconds to scene milliseconds, bounded by INT32_MAX and wrap-safe deadlines. It does not claim recovered retail unit semantics.
- Default-on CVar `wxlTimedEnchantModels` controls kit-derived model timeline playback. Set0 to release those kit models on the next scene pump. Native DBC and direct ItemVisuals/XEffect models remain. Overall wxlEquipmentAttachments0 also releases timeline owners.
- Variation randomization, nonzero unknown ModelAttach/EffectName flags, positioners, anim-kit timelines, texture substitutions and dissolve remain refused. No speculative deterministic random distribution, flag mask, or retail timing equation was invented. ProceduralEnchants is a separate concurrent stage.
- Static offset/Euler semantics are native source behavior. Start/loop/end phase ownership and delay seconds are an explicitly bounded source compatibility port, pending combined compile and gameplay/pixel acceptance.

Merge notes:

- WxlEquipmentAttachments adds EnchantModelTimeline.cpp; no global feature define or DX11 change.
- CM2Model.h/.cpp and CM2Scene.h/.cpp add owned state/list and detach/destructor/pre-worker pump seams. Merge root's procedural-owner members/calls alongside these.
- EquipmentAttachments::ApplyItemVisual now computes graph/subclass before cleanup and has a SameTimedItemVisual early return. The procedural stage's Release-at-entry/Apply pair must execute together before that early return, or both must remain after it. Do not clear procedural state and then return before reapplying it.
- ItemVisualGraph adds ItemGraphVisualGeneration for its own publish/clear. Merge procedural graph additions separately; preserve their immutable catalog data.

Primary schema reference: https://raw.githubusercontent.com/wowdev/WoWDBDefs/master/definitions/SpellVisualKitModelAttach.dbd . Existing typed native CM2Model sequence, CM2Scene worker ordering, and CEffect transform implementation are bound in manifest input hashes.

Native clone seam: CM2Model::Initialize(source) copies source attachment children. The stage excludes source-owned timeline children from that generic clone loop and copies only current active recipes into a destination owner, with a new parent epoch/start clock. Pending and ending children therefore cannot become permanent static clones; ordinary native attachment copies are untouched. Allocation failure drops the optional cloned timeline rather than producing unowned child copies.
