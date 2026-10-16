Integrated native source implementation; combined build/acceptance tracked separately.

# Native WXL donor mesh-family breadth candidate, 2026-10-16

Pending source-only stage based on native HEAD 01e28402e9f6f399a5997430e82bb8d710ebdfdd. No shared source edits, builds, tests, shader compilation, game launch, installs, or publication. `candidate/` contains three complete changed files; `baseline/` preserves their exact original bytes. Apply `mesh-material-breadth.patch` by hunks after checking input hashes; preserve concurrent particle, creation, movement and appearance work.

## Implemented behavior

The native source previously admitted only one source21/0x8015 batch, one section and exactly two model textures. This change implements the actual WXL donor `Skin.cpp::DownConvertBatch` compatibility group 21/33/35 (`0x8015`, `0x8021`, `0x8023`) across up to 32 source-indexed batches, 32 sections and 64 model file textures. Each batch still uses exactly two file-texture stages and no secondary material layer. A skin must be wholly covered by the bounded family; mixed unsupported source batches are not silently substituted.

This is a WXL donor compatibility implementation, not a claim of verified retail shader equations. The donor's original switch explicitly maps all three IDs to packed4011 with the custom AddAlpha VS/PS. Its independently verified `SourceMaterial.hpp` table classifies retail effect21 as ModMod/EdgeFade and effect33 as ModMod2x/EdgeFade, and does not verify effect35. The generated AddAlpha bytecode and HLSL remain the existing exact donor programs. No new pixel equation was invented to conceal that distinction.

The donor has a separate exact Entropic Rift source33 correction in `RiftMeshPolicy.hpp`/`Skin.cpp`: `spells\\cfx_priest_entropicrift_areatrigger` source0x8021 uses Mod_Mod2x rather than AddAlpha. This candidate preserves that evidence as an explicit exclusion. The exact stem plus .m2/.mdx forms are matched case-insensitively with slash normalization. The exclusion runs both during source preparation and live draw validation. That named effect remains outside this AddAlpha path; no Rift shader parity is claimed.

## Actual conversion and draw consumption

`Source` now owns the source model path, source model/SKIN fingerprints, section/texture counts, and a vector of immutable `BatchSource` records. Every record retains its exact original 24-byte batch, two texture/TXID identities, two weight/transform references, flags and blend. No global pointer registry or aggregate grant is added. `ModelAssets` supplies its owned source path to the loading seam; the source recipe retains its own copy.

Preparation validates every section, every referenced vertex and every batch before returning a private converted-input clone. All geometry still requires one inline model bone; every section has a one-bone palette pointing to bone0 and one influence; every referenced vertex has weights255/0/0/0, boneindices0 and skin properties0. Materials require blend7 and native-supported flags. Model version274, one named skin, D3D9/9Ex native cached SM3 capabilities, source bounds, file textures, nonzero TXIDs and complete stage lookup entries remain required. Existing Materials controls remain authoritative.

Only the private SKIN clone changes each admitted source shader field to donor packed4011. The exact original companion still supplies native PrepareSkin's source comparison. The vendor generic admission policy and `ConvertForNativeRuntime` entry remain unchanged. The converted companion must preserve source batch count, order, section/geoset identity, sanitized flags, priority, color/material/layer identity and both-stage source lookup indices. A mismatch rejects the prepared model.

At native DrawBatch, the renderer locates the actual current batch within the current native skin, selects the corresponding immutable source record, verifies section ownership/count, source family, exact live material, native finalized shader0x11/UV combo4/two stages, palette mapping, texture descriptors and both stage lookup references, and then binds the existing exact donor VS/PS. Each batch consumes its own references; one batch cannot borrow the first batch's material contract. No native batch reordering or splitting is assumed. The admitted zero material-layer contract means native SubstituteSpecializedShaders returns before its layer-combination rewrite; converted and live pairing checks still fail closed.

The root's persistent device shader-list ownership, unique program names, exact byte reuse validation, native reset recreation, GxDevDestroy cleanup, RsPush/RsPop and correctly typed D3D9/9Ex state synchronization are retained unchanged. The root's native DrawBatch ownership seam also remains unchanged: a privately prepared model is suppressed if its custom draw cannot execute, including fixed-function/backend failures, instead of falling back to a different stock equation. This candidate does not add particle programs, declarations, blend factors, callbacks or render hooks.

## Remaining limits and acceptance

DX11/OpenGL, multiple model bones, weighted palettes, multiple skin profiles, secondary material layers, mixed unsupported mesh families, three-stage/TXAC materials, refraction, shadow/projected/doodad batches and the named Rift33 exception remain unsupported by this path. Death and Decay's source0x4013/three-texture/TXID+TXAC contract remains separate; its fade/mask/detail programs are not globally enabled. Material blend7 and particle Screen behavior remain distinct.

No actual source21/33/35 fixture was rendered or runtime-accepted here. Parent gates include the full native build/link and exact source/build evidence; real admitted multi-batch assets; per-batch sampler/UV/material validation; camera/edge-fade comparison with the WXL donor; interleaved native/custom draws; device reset; model unload/reload; negative Rift/backend cases; performance and gameplay acceptance. Source breadth is implemented; retail equation parity is not established. No accepted runtime, MPQ, catalog or publication state changes follow from this source candidate.

Rollback before integration means leave this stage unused. After integration, reverse only the exact reviewed hunks while preserving unrelated work; no reset or clean is appropriate.

## Named Entropic Rift native combiner

The separate donor RiftMeshPolicy correction is now integrated at the source-owned companion loading seam: exact canonical `spells\\cfx_priest_entropicrift_areatrigger` MD21/v274 source0x8021/two file-texture stages, unlit material flags and blend2/7 become private packed0x4014 input. The actual native converter produces two-UV0/1 Mod_Mod2x, which native M2Shared selects as Combiners_Mod_Mod2x. All original model/SKIN bytes remain owned and native PrepareSkin still checks the exact original companion before publishing converted bytes. Unsupported metadata/refusal commits no private clone. Other material rows remain unchanged and must independently pass the existing converter. No one-bone/custom shader gate is needed because this path uses the ordinary native weighted draw.

The exact named exception remains excluded from AddAlpha. This implements donor pixel-combiner compatibility only; its missing retail edge-fade vertex stage, depth effects and particle families are not claimed as complete. No global material33 mapping, source pointer registry or new backend hook is introduced.
