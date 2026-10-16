Current source integration: the candidate described below has been merged into the native modular checkout. Exact combined build and runtime identity are recorded by release-control, separately from gameplay acceptance. Original worker staging notes follow as implementation detail.

# Native creation choreography source candidate

Status: pending source integration, unbuilt. No build, test, install, binary export, acceptance, packaging, or publication was performed by this worker. Root integrates the overlapping wiring and runs one combined build. The manifest captures actual observed HEAD and per-input bytes; it does not bind any previously built binary to this source.

The two owned source files are `candidate/WXL/CreationEffects.hpp` (C++17 public declarations) and `candidate/WXL/modules/creation/CreationEffects.cpp` (C++20 private implementation). `integration.patch` includes these plus narrow integration deltas. The other five candidate files are complete review snapshots of shared integration files, not permission to overwrite newer files. Apply their changes by reviewing patch hunks against current bytes.

## Implemented candidate scope

The exact donor VisualEffect aggregate, attachment IDs, model paths, scales, target offsets, textures, nine-class phase functions, Blizzard/Priest/Imp/Rain emitters are taken from `E:/WXL1.1/extensions/wxl-character-creation-preview/src/CharacterCreationPreview.cpp`. Both E donor and x64 hook donor hashes are recorded. The portable recipes contain no injected function calls, x86 layouts, fixed executable addresses, or exception-based pointer probes. Logging macros are removed. Native phase timing tables translate the donor's dedicated Lua timelines relative to its 3-second animation-base window.

| Class | Authored candidate coverage | Unverified/deferred contract |
|---|---|---|
| Warrior | Battle Shout, Thunder Clap, Slam/Cleave impacts, Bladestorm | Mounted models, per-race animation support, visual placement |
| Paladin | Consecration, persistent wings, Crusader Strike, Holy Light precast/impact, Kings/bubble | Native shader/particle parity, animation timing and shield visuals |
| Hunter | HD wolf/skin, KneelStart/Loop/End trap sequence, Load/Hold/FireBow, aimed/explosive missiles, Flare | HD wolf asset availability and bow/equipment visual parity |
| Rogue | Backstab, Eviscerate, Vanish, hidden stealth interval, Shadowstep, Fan of Knives | Visibility/camera/frame timing and race animation support |
| Priest | Three Penance bolts, shield open/hold/release, Shadowform, Psyfiend, Mind Flay hand/head/target effects, three shadow bolts | Actual unit-to-unit Mind Flay ribbon is unsupported; no substitute beam geometry. Full visuals remain unaccepted |
| Death Knight | Ghoul/skin, persistent Death and Decay, Frost/Blood strikes, Death Coil, Blood Boil | Mounted asset and native effect/animation playback parity |
| Shaman | Ghost Wolf model/skin, return, four persistent elemental totems, Windfury/fire strike, lightning/lava precast/releases | Ghost Wolf force-visible sibling composition and class-specific weapon/attachment parity |
| Mage | Pyroblast/Frostbolt, Water Elemental + two texture slots, Blizzard 5/sec for 16 impacts, Arcane Explosion | Mounted models/textures and native render timing/particle parity |
| Warlock | Persistent Imp with independent prepare/release Firebolt cycles, Shadow Bolt, exact retail Chaos Bolt path/InFlight144, 12 Rain of Fire falling missiles | Actual Chaos asset availability, mounted dependencies, shader/particle parity |
| Druid | Unsupported; no guessed forms or abbreviated substitute timeline | Full authored 17-phase shapeshift/flight/decay lifecycle needs independent native form ownership |

These are translated authored phases, not a claim that any whole class is complete or visually accepted. Missing mounted models or required monster textures skip the visual. The bounded existence memo uses real `SFile::FileExists`; .mdx names canonicalize to .m2 without guessing aliases. Newly created effects remain alpha-zero until native load completion and exact normalized `CM2Shared::m_fileName` validation, so errorcube fallback is rejected. Asset changes require a restart to refresh the bounded memo.

## Native ownership and lifecycle

`CM2Scene::CreateModel` owns each effect's original reference. Native `AttachToParent` adds a parent reference; `DetachFromParent` drops it; full cleanup releases the original reference. No actor/component reference is owned. Component serial and native model handle guard borrowed actor writes. Lighting uses the actor's native callback/argument; replacement textures use native `TextureCreate`, `ReplaceTexture` and `HandleClose` ownership. Bounded hidden warm models preload the donor's early shield/missile dependencies and are released with the session.

Hand/head/chest/weapon effects attach with the donor's real native attachment IDs, skipping missing attachments. Base-relative effects are siblings under the actor's background parent, with transforms converted through actual animated actor/background matrices. This avoids feeding animated actor bone-zero motion into authored trajectories and avoids hidden actor alpha making Ghost Wolf invisible. Native actor-facing, scale and scene ownership remain authoritative. Capture-hand missile origins convert through native attachment position and actor inverse world transform once.

Hook `StopCreation()` before actor retirement, background replacement, creation hide, shutdown and Lua context teardown. The staged `CharacterCreation.cpp` patch calls Stop before publishing a replacement component (after the candidate initializes successfully), preserving the old actor on failed replacement. Race/sex/class changes rebuild through this seam. Idle drives effects after native `CGlueLoading::Idle`. The runtime also compares actor pointer, component serial, model handle and race/sex/class each tick.

Alternate appearance editing explicitly suspends through `alternate_appearances::IsAlternate()`; returning starts a fresh timer. Existing-character customization is gated by `m_existingCharacterIndex`. Native Lua registration installs a private setter and source-owned exact `CharacterCreate.CustomizationFrame.PlayToggleAnim` wrapper: stop before the original method, read `.show` after it, poll `.show and IsShown()` and rewrap after live UI method/frame replacement. Entering customization reasserts Stand once after 200ms; closing starts a new timeline. UI registration resets the bridge on context rebuild. No Lua pointer offsets are used.

## Integration

- Add `WXL_ENABLE_CREATION_EFFECTS` option default ON and compile definition through the existing option loop.
- Add `WxlCreationEffects`/`WXL::CreationEffects` object module, dependencies runtime/appearances/storage, and object collection into WxlNative.
- `RegisterFeatures()` calls `creation_effects::Register()` to register `wxlCreationEffects`, default ON when compiled. Native global OFF and CVar OFF gate behavior.
- Add the module introspection entry `creation-effects` using `Enabled()`.
- Character creation script registration calls `InitializeLua`; unregistration stops and unregisters the private Lua function.
- Merge the five shared integration files by hunks; other workers may have concurrently added module wiring.

## Acceptance and rollback gates

No build/source binding exists until root compiles the merged exact source. First run should exercise native load failure/fallback suppression, every class phase, all mounted race/sex animation variants, rapid class/race/sex switching, hide/show, customization open/close, alternate-form switching, UI rebuild, and actor/background destruction under active missiles/emitters. Verify all original references and parent references retire without stale access; verify lighting/textures/particles/ribbons in the target native renderer. Inspect missing-asset cases rather than approving an effect solely because its phase executes.

Rollback is source-only: remove the two owned files and inverse only the listed integration hunks, preserving unrelated dirty source. Nothing has been installed or published. Root must include this source stage, its manifest and unresolved gates in the combined pending release-control intake; no independent candidate promotion is authorized.
