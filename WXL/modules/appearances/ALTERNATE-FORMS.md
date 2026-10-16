# Native alternate appearances

Integrated native-source owner. Combined compilation and local runtime staging are recorded in release-control intakes; runtime, appearance and server acceptance remain separate. This file describes the current implementation, not an unapplied stage.

## Concrete behavior

- Wire race 12 uses primary retail family 22 and alternate 23; race 17 uses 76/70; race 30 uses 75/52. Authority is chosen by the actual native model's exact resolved recipe path. The primary still must match mounted ChrRaces/CreatureDisplayInfo/CreatureModelData. An alternate must match the explicit wire-family mapping, its real DB2 recipe, and an actual mounted MD21 source file.
- The normal native `CCharacterCreation::m_character` stays the primary actor. A separate owner creates and retains the alternate component/model. Form switching changes the preview scene attachment and the UI editor target; it never swaps the canonical native creation pointer.
- Each actor owns its own preferences, option overrides, renderer, child models, texture sheets and native lifetime. First construction initializes the alternate's own defaults; it does not mirror primary choices. The existing five creation controls and explicit option UI route to the active actor. Alternate randomization selects from that actor's actual authority.
- Worgen Human supplemental Eye Color follows the donor/server rule: it is appended from Human family 1 only when family 23 and family 1 resolve to the exact same model FDID. Its selected eye material/geosets enter the alternate renderer, and its option ID enters the canonical alternate array.
- Capture is read-only and requires two current, independently bound actors. It records both component serials, native model handles, choice revisions, and the owner generation. Both option arrays must be nonempty and canonical within the existing 64-choice-per-form wire limit. A prepare permit is invalidated by either form changing or the preview owner changing.
- Snapshot restoration selects the correct canonical array by the actual component model family. Creation restoration validates and allocates both option states before publishing either actor's update. It does not overwrite a shared model recipe.
- Native create/customize sends keep the primary's classic fields. The negotiated appearance prepare carries both independent canonical arrays. Required modern forms cannot save with an empty alternate array. The disabled/classic path retains its existing behavior.
- Native main-thread creation/retirement seams release the alternate before old actors/scenes are retired. On visibility/session changes, the code refinds the actual primary identity before touching it. Public headers remain C++17-compatible; implementation files use the module's existing C++20 setting.

## Retention and failure behavior

Actual alternate option/choice pairs are retained by native session, GUID, wire race and sex when an owner retires. Recreated actors restore those exact IDs through the new actual-model authority. If capture or restoration fails after a schema/model change, the replacement alternate is not published with defaults. Session changes clear these retained rows. Retention is bounded to 64 contexts; an exhausted retention session refuses new alternates instead of dropping prior selections.

A failed alternate construction is memoized for that exact primary component serial/model handle/session/GUID/race/sex/class context. It does not recreate probe actors every Idle tick. A new owner/context is needed to retry. Actual renderer readiness and both current actors are required before capture, so initial construction defaults cannot masquerade as a saved actor.

## Remaining limits and review gates

- This is source integration only. The parent must perform the combined compile and resolve any compiler findings before producing a candidate executable. No runtime/UI/save/reconnect acceptance is claimed.
- Alternate families must actually exist for the requested sex in mounted tables and assets. Missing forms, layouts, textures or exact model matches fail closed. No opposite-sex or synthetic model substitution is attempted.
- Alternate preview synchronizes garment, shoulder, cape, quiver and main/off-hand display IDs as the primary changes. Native item loaders own attachment models/textures and retirement. Off-hand shield mode follows the actual primary shield attachment. Helmet suffix and legacy hide-mask race come only from a unique mounted native default matching the alternate exact model path; ambiguous/missing families omit the helmet. Primary forms retain their own native race policy. This adds native display-ID equipment, not modern DB2 item-resource or enchantment parity.
- Server authority must use the matching catalog and donor supplemental Worgen Eye Color rule. This handoff does not change or publish server data. Snapshot query, hello, prepared permit, timeout and retry bounds remain the existing native session contract.
- The stage reuses the current DB2 option/choice authority semantics. It does not add requirement-condition filtering or a second appearance catalog.
- Rebuilding an actor with retained IDs that no longer validate blocks that alternate until a new valid owner/session is supplied. There is no UI reset-to-default action in this patch; preserving edited state takes precedence over silently defaulting it.

## Ownership

`AlternateAppearances.cpp` owns the alternate component. `RetailChoices.cpp` selects exact model-family and equipment authority. Native creation/session seams retain canonical primary ownership and release alternate actors before scene retirement.
