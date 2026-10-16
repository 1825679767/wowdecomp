# Native creation Mind Flay beam

Frozen source-only stage against observed native HEAD `d316528f40ae27893d3b3eb30d67102cc074bc8f`. The parent had advanced HEAD from the initial assignment while this work was underway; exact independent before-file hashes, the source dirty diff and hashes of native behavioral references are captured in `manifest.json`. No shared source writes, builds/tests, game/server actions, installs, packaging, publication, new Downloads or DX11 work were performed.

## Concrete scope

The E donor and hook-worktree CharacterCreationPreview both explicitly state that the true Mind Flay beam requires the engine's between-unit ribbon/chain path. They author three ShadowBolt missiles at 800ms intervals plus a ManaFunnel impact at actor-local `(-6.85,0,5.45)`; neither donor contains a dedicated custom beam renderer. The parent explicitly authorized filling that missing creation presentation through the existing typed native spell-chain implementation, rather than inventing beam assets.

This candidate adds a dedicated, default-ON `creation-beams` module and `wxlCreationBeams` CVar. The existing Priest channel phase at 6200..9400ms starts a 3200ms beam request. Existing donor-authored hand/head effects, impact model and missile stream remain intact. Phase clear/stop cancels the beam. Other classes, retail spell447445 and native world spell visuals are unchanged.

## Actual metadata and renderer

The beam resolves mounted native Spell 15407 through the existing GetSpellVisual function, including native reduced-violence visual selection, then its channel SpellVisualKit. Only native procedural types 0/12 supply chain IDs from CharParamZero, exactly as CGUnit_C::CreateProceduralEffect does. CharParamTwo controls persistence. No visual, kit, chain-effect or texture ID is hardcoded. This presentation has one fixed target location, so native multi-target order/reverse selection does not add another endpoint.

Each root chain and its bounded encoded combo words are resolved from SpellChainEffects. Complete native rows and exact mounted texture paths are copied into stable owned objects. The manager borrows those owned rows and retains native texture references. No DB-row pointer, mutable global alias, x86 offset, injected hook, generated model path, shader approximation or replacement renderer is introduced.

The source position follows the real animated model. It uses the native visual cast offsets with the native Y sign, raw-attachment flag, exact native 16-entry effect attachment mapping, special weapon socket selection for effect attachment 13, and the native `$CSL` cast event fallback. An actor without a valid metadata attachment or cast event does not get a fabricated unit height. The target follows the donor's accepted impact center through the actor's actual animated world transform; no additional target offset moves it away from that authored impact.

The real native CLightningManager builds and draws the geometry, textures, blend/render-layer state, waves, arcs, pulse/flicker behavior and native bounded joints. Source/target positions update after the matching Glue scene has animated. The module calls the manager inside CSimpleModel::RenderModel after opaque/translucent M2 passes, while the matching Glue camera, viewport and enclosing FFX are still active. A scoped identity world base lets the native manager subtract that camera exactly once; its native render-state push/pop remains in use.

The existing CLightning renderer hardcodes World::FrustumCull. The parent approved a narrow typed `worldCull=true` default parameter on CLightning::Render and CLightningManager::Render. All existing world callers keep their original default. Only this Glue presentation passes false, avoiding unrelated world-camera culling. No World, fog or water source file/state policy is modified.

## Bounds and lifecycle

At most twelve chain rows are retained. Combo reads consume only complete binary-safe words, at most 48 root bytes. Procedural IDs, scalar metadata, hierarchy strides, durations, render layers, blend enum values and divisors are validated before native manager use. Native joint count remains capped at 1000; unsafe metadata is refused instead of patched into different values. A queued request retries metadata/resources every 250ms within its actual phase lifetime. Texture references remain retained while the GPU resource becomes ready; unresolved textures stay invisible. Frame integration is bounded to 250ms while phase expiry uses real elapsed time.

State retains one native actor reference and verifies the current creation component serial, model handle, race, sex, class, scene, visibility, alternate presentation and existing-character mode before rendering. No saved component pointer is dereferenced. The creation module's existing stop-before-retire seams now also stop this owner, including hide, customization, UI rebuild, race/sex/class/model changes and phase transitions. A scene pre-destruction seam releases the manager/texture/actor references before CM2Scene's force-release loop. The manager retires before its owned row strings or actor reference.

## Files and integration

Ten complete candidate files are provided; merge the patch if any source-before hash changed:

- New `WXL/CreationBeams.hpp` and `WXL/modules/beams/CreationBeams.cpp`.
- `WXL/modules/creation/CreationEffects.cpp`: authored phase start and clear/stop lifecycle.
- `Engine/Source/Frame/CSimpleModel.cpp`: matching Glue camera/viewport render seam.
- `Engine/Source/Model2/M2Scene.cpp`: scene pre-retirement cleanup.
- `WoW/Common/Lightning.h/.cpp`: optional typed world-cull selector, default preserving native world behavior.
- `WXL/CMakeLists.txt`, runtime `NativeFeatures.cpp` and `ModuleApi.cpp`: independent object module, default-ON switch, CVar and module declaration.

No M2Model.h/.cpp/skinning file is touched, preserving the parent's concurrent collection-retarget changes. `stage.py` is the source-only generator/provenance aid; preserve the frozen output and do not rerun it against integrated source.

## Remaining acceptance and limits

No build or tests were run. The parent performs the combined compile. Actual mounted spell/channel/chain rows and textures, cast attachments/events across races/sexes, camera projection, target placement, blending, texture streaming, phase cancellation, customization, alternate presentation, CVar disable and scene retirement require runtime visual acceptance. Missing mounted metadata/assets fail closed. This is source implementation evidence, not accepted gameplay or full beam parity.

The module is intentionally limited to the actual Priest creation channel. It does not add world-unit beams, new spells, guessed retail aliases, multi-unit chain target acquisition, UI beam controls or server actions. Other unsupported creation/spell beam effects require their own authored metadata and owner seams. Generic native SpellVisuals/LightningObject gameplay behavior is preserved.
