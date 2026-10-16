# Native single-texture particle cutoff stage - 2026-10-16

Source-only stage. Root applies/integrates the patch and performs the combined compile. No shared source edits, tests, game probes, shader compilation, installation, commits or acceptance promotion occurred. `git apply --check` passed when staged. `manifest.json` records exact base/staged file hashes and patch identity.

## Actual rendering path

This stage implements nonzero EXP2 particle-lifetime cutoff through the existing native shader abstraction, using explicit per-particle indexed draw grouping. It does not widen the particle vertex ABI or replace native shaders with donor-address-bound programs.

1. ParticleCompat reads and validates the source UNORM16 lifetime cutoff arrays. Nonzero tracks require strictly increasing times, neutral per-particle TXAC, neutral zSource, single-texture sprite emitters and empty model/child names. It retains existing EXPT/EXP2 multiplier baking and EXP2 precedence. Model/emitter rows are never omitted or reordered by this stage.
2. Immutable owned tracks are stored in ModelAssets under the same native CM2Shared owner as adapted model data. Each emitter gets a shared_ptr at native InitializeLoaded. Existing Params copy construction propagates the reference to cloned emitters. Sidecar lifetimes therefore cover model reload/destruction and emitter clones; no borrowed chunk bytes remain.
3. Native BuildParticleVertices samples cutoff at each actually rendered particle's age/life. Life uses the exact existing native signed life seed, life variation and minimum0.001 calculation. Samples are recorded after BuildVertex succeeds, retaining native depth sorting and excluding twinkle-skipped particles.
4. Affected emitters bypass cross-emitter pooling. The ordinary emitter path still builds one native24/36-byte vertex buffer with existing billboard, scale, spin, atlas UV, lighting and fog behavior. Per-particle cutoffs correspond to sequential4/8-vertex native shapes in that buffer.
5. DrawParticleCutoffVertices binds the same native vertex/light permutation as SetDefaultShaders(0), but explicitly selects the existing floating-alpha-reference pixel permutation (pixel bit8). SetAlphaRef writes pixel constant2.w, and native Gx Draw submits sequential particle index slices. Only adjacent equal thresholds coalesce; original particle order is preserved. The effective test is max(existing native material threshold, sampled source threshold), preserving both existing and source cutoff constraints.
6. Alpha reference and default shader selection are restored through CShaderEffect's own state API after drawing. No raw D3D calls, fixed addresses, hooks, alternative OpenGL path, or weather/fog/liquid changes are introduced. Mesh blend7 and particle Screen blend ownership remain untouched.

## Shader assets and readiness

The required floating alpha-test permutations already form part of native CShaderEffect's sixteen-entry pixel shader table. SelectShadowShader/SetShaders explicitly identify bit8 as the shader-alpha-reference path; SetShaders disables hardware AlphaRef and supplies constant2. Vertex selection follows native SetDefaultShaders(0).

No new shader source, bytecode asset, external compiler or shader build hook is required by this stage. It reuses the native Particle/Particle_Unlit shader families. BOTH selected native vertex and floating-alpha pixel CGxShader objects must exist and report Valid() on the active backend before drawing. Missing/invalid variants or fixed-function mode refuse the affected emitter draw; there is no fallback to an incorrect emitter-wide threshold or8-bit cutoff. Runtime asset availability has NOT been verified, so rendering support/visual acceptance is conditional on those exact native assets. This is source integration, not a claim that an absent compiler or missing shader asset was supplied.

## Build/source hooks

Apply native-particle-cutoff.patch, or merge its small hunks if parallel root work changed the surrounding files. review/ holds full files for inspection only; do not overwrite shared files wholesale with review copies.

Add WXL/modules/particles/ParticleCutoff.cpp to WxlParticles. New public header is WXL/ParticleCutoff.hpp. Existing WXL/source-root include directories suffice. Existing Gx/Services/ShaderEffect code and WxlNative object facade resolve symbols; no new external library.

Patch touches these exact seams:

- WXL/ParticleCompat.hpp and modules/particles/ParticleCompat.cpp: optional cutoff ownership output and guarded nonzero capture.
- WXL/modules/m2/AssetCompat.cpp: ModelAssets sidecar vector, pass output during candidate adaptation and ParticleCutoffFor getter.
- Services/ParticleSystem2.h: native runtime Params shared_ptr plus rendered threshold vector. No on-disk M2 struct gains fields.
- Services/ParticleSystem2.cpp: retain one cutoff per successfully emitted native particle shape and invoke cutoff draw path.
- Model2/CM2SceneRender.cpp: prevent both directions of cross-emitter batching for affected emitters.
- Model2/M2Model.cpp: one native emitter-initialization assignment only; merge this hunk without replacing other workers' skinning changes.

## Bounds and intentional limits

Cutoff keys max4096 per emitter; owned cutoff sample bytes max64MiB per model. Native vertex/index budgets remain16384 vertices per draw buffer. Shape count and cutoff count must match. Raw source and target emitter count must match as before. Nonzero cutoff times must be strictly ordered (duplicates refuse); zero-only ramps retain previous permissive ordered-domain handling and ordinary native draw path.

Single-texture native sprites/head-tail shapes are the supported scope. Multilayer/TXAC UV composition, refraction, geometry/child model particles, nonzero zSource and fixed-function rendering remain unsupported for nonzero cutoff. Native replacement-color behavior remains as documented for multiplier baking. This stage does not claim retail multilayer equations or animation parity outside its stated subset.

Per-particle threshold slices may increase draw count up to the native particle count when adjacent thresholds differ. This deliberate bounded path keeps order and exact floating thresholds; a future extended shader/vertex ABI could reduce draws. Frame/performance and actual shader-asset acceptance remain open after root's compile.

## Source contract evidence

Primary parser documentation: WhiteoutLib M2 specification section13.4 describes EXP2 per-lifetime alpha test and EXP2 precedence over EXPT:
https://github.com/sc2-arcade-watcher/WhiteoutLib/blob/libmodkit/docs/M2_FILE_FORMAT_SPECIFICATION.md

Primary library record documentation describes alpha cutoff sampled by particle lifetime and the scalar fields:
https://skarndev.github.io/wowlib/python/m2/records/

Native rendering contract is the checked-in ShaderEffect.cpp SetDefaultShaders/SetShaders/SetAlphaRef and ParticleSystem2.cpp InterpolateAllTracks/StepUpdate/BuildParticleVertices/RenderParticleVertices, not donor binary offsets.

## Pinned parser verification of normalization

Read directly from WhiteoutLib commit a88224a9 (not inferred from the WotLK `fixed16` class):

- include/whiteout/models/m2/types.h:108 defines ParticleAnimationTrack timestamps as vector<unorm16>.
- include/whiteout/models/m2/structures/extensions.h:10 defines ParticleEmitterExtension alphaCutoff as ParticleAnimationTrack<unorm16>.
- include/whiteout/common_types.h:197 defines unorm<UInt>::operator float as raw / numeric_limits<UInt>::max; unorm16 aliases unorm<u16>. Both cutoff timestamps and cutoff values therefore use65535, not the native ordinary alpha-track signed32767 divisor.
- src/whiteout/models/m2/binary_parse_visitor.h:267 visits timestamp and value arrays independently; extensions.cpp:10 visits the three scalars then alphaCutoff. Offsets remain relative to their EXP2 payload.
- src/whiteout/models/m2/parser.cpp:150..168 applies EXPT and then replaces with EXP2 when present.

Pinned primary links:
https://github.com/sc2-arcade-watcher/WhiteoutLib/blob/a88224a9/include/whiteout/models/m2/types.h#L108
https://github.com/sc2-arcade-watcher/WhiteoutLib/blob/a88224a9/include/whiteout/models/m2/structures/extensions.h#L10
https://github.com/sc2-arcade-watcher/WhiteoutLib/blob/a88224a9/include/whiteout/common_types.h#L197
https://github.com/sc2-arcade-watcher/WhiteoutLib/blob/a88224a9/src/whiteout/models/m2/binary_parse_visitor.h#L267

## Indexed draw and restoration details

All slices retain the SAME native Gx vertex buffer and native shared quad index buffer. For particle ordinal b, start index=b*indicesPerParticle; minimum vertex=b*verticesPerParticle; maximum vertex=(endOrdinal*verticesPerParticle)-1. Native Gx alone adds its current stream/index buffer base offsets. No manual byte/vertex base is added or duplicated. Native4-vertex/6-index sprites and8-vertex/12-index head-tail shapes therefore address consecutive native quads correctly.

No blend/depth/cull/color-write state is overridden by this helper. Native SetupMaterial owns those states, including Screen versus mesh7. Fog RGB constant components stay unchanged while only alphaRef w changes. Existing default shader selection and saved alpha reference are reapplied through CShaderEffect after the final slice; RenderParticles restores its original view using the existing native caller path. No private OM or device-state cache is introduced.

## Converter ordering and original source admission

The current vendored M2Bytes::Convert chunk scan extracts MD21/TXID/AFID/SKID and does not impose an EXPT/EXP2/TXAC auxiliary particle policy. Thus nonzero cutoff reaches the native module after Convert; no vendor/external gate was widened, no grant bit added and no source chunks stripped. ApplyParticleExtensions rechecks exact original/target emitter counts and reads the original MD21 particle array using kParticleStrideModern. Affected emitters must have the multitexture discriminator clear BOTH in original source flags and normalized native flags, before accepting a cutoff sidecar. Packed-layer metadata therefore cannot disappear through normalization and accidentally enable this single-texture path.

Root integration keeps the native-facing cutoff API C++17-compatible using const vector reference; module-only conversion remains C++20.
