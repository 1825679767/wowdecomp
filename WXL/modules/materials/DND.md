Integrated source candidate; source/build and runtime states recorded separately.

# Native Death and Decay material candidate, 2026-10-16

Pending E-only source stage, initial native HEAD 6fcc461e7683cf5f245203d0292859a535df24e9. No shared source edits, builds, tests, shader compilation, game launch, runtime install, packaging or publication. `candidate/` contains six changed integration files, two new implementation/header files and six byte-for-byte donor shader headers. `baseline/` retains the exact changed input files. Apply the source patch by hunks after comparing hashes; preserve concurrent root work.

## Implemented source family and kernels

This implements the fully sourced, narrowly named `spells\\cfx_deathknight_deathanddecay_state.m2` donor experiment, including its actual Fade, Colour, Mask and Detail kernels. It is not generic retail three-texture/TXAC support and does not claim accepted pixel parity. Mask/Detail remain the donor's captured circular third-map approximation; Detail modulates both RGB and alpha with that artwork. The generated VS is the captured native weighted VS plus the donor's untransformed UV0 mask output; its generator is recorded as an input but was not run.

The build option `WXL_ENABLE_DND_MATERIALS` defaults ON and is defined privately on WxlMaterials. `wxlDndMaterialMode` defaults to **4, Detail**, as authorized for later broad testing. Modes are 0 OFF/rollback, 1 Fade, 2 Colour, 3 Mask, 4 Detail. These are immutable load-time source recipes; changing mode requires world/model reload. A mode mismatch suppresses an already prepared custom pass instead of displaying a different stock equation. The donor originally exposed separate default-off trial controls; this native default-on choice is intentional user-authorized experimental source compatibility, not an acceptance claim.

Admission requires D3D9/9Ex with cached native SM3 capabilities, enabled Materials, exact named MD21/MD20 v274/globalflags0x3090, exactly eighteen source textures/TXIDs, one named skin and three source batches. The first two batches must be source0x4013/three stages with materialflags0x13/blend7 and the exact donor TXID triples:

| Pass | First map | Second map | Third map |
| --- | --- | --- | --- |
| 0 | 1603902 | 1522978 | 1603903 |
| 1 | 1601200 | 1620780 | 1612964 |

Mask/Detail additionally require the donor batchflags0x8c, source material TXAC `{1,1}`, texture flags `{3,3,4}`, populated first two transform references, identity third transform and neutral/absent third weight. This stage retains complete source batch bytes, three texture/TXID/flag/weight/transform identities, captured TXAC, model/SKIN fingerprints, source path and selected mode in immutable model-owned metadata. All three batches must have no secondary material layer so native layer-combination rewriting cannot silently reorder the source mapping. The third material cannot share either corrected material index; source preparation and material application both enforce that boundary.

## Actual native data and draw flow

The owned source-loading seam supplies a private SKIN clone in which only the first two stage counts change from three to two. Shader0x4013 and original batch/material/lookup identity remain captured. Native conversion supplies the existing Mod_Add setup, native two-stage UV0/UV1 mapping and weighted geometry/palette handling. The exact original companion bytes still supply PrepareSkin's input comparison. ConvertedMatches verifies the first two native serialized records against their source originals and the expected conversion shape.

After PreserveMaterials, ApplyPreparedMaterials changes only the two explicitly identified, privately owned native material rows from blend7 to donor live blend4. It checks source flags and bounds first and rejects any shared index with the native third pass. No global blend table changes. This is necessary because the donor GPU contract is SRCALPHA+ONE, while native blend7 is ONE+INVSRCALPHA. Native setup therefore computes the corresponding stock blend/fog/material state before custom shader substitution.

The native DrawBatch ownership seam is now per batch. First and second DnD passes belong to the custom consumer; the third returns to the original native drawing branches, including their own shader/vertex handling. Existing AddAlpha models retain their prior ownership behavior. The DnD consumer verifies live source-indexed batch identity, section identity/count, flags/material/blend, finalized shader0x13, stage/UV mapping, texture handles and stage lookups.

Before any replacement, it synchronizes native Gx state, validates the selected native shader objects, retrieves the actual bound D3D VS/PS and compares GetFunction bytes against the exact donor capture, and reads the real owner-thread D3D state to require SRCALPHA, ONE, ADD, alpha blending enabled and depth writes disabled. Temporary COM read references are released immediately. A mismatch suppresses the owned pass and emits a bounded diagnostic. No guessed native shader contract is accepted.

Fade/Colour substitute only their exact donor PS. Mask/Detail also select the exact donor mask VS, resolve the third model texture through native TextureGetGxTex, and bind it to native texture stage2. The sampler gets the donor clamp/linear/miplinear/zero-LOD/non-sRGB state. Direct sampler changes are individually captured/restored inside native RsPush/RsPop and correctly typed D3D9/9Ex synchronization. No CGxTex flags or VS/PS constant registers are overwritten. A sampler restore failure disables custom DnD passes for that session.

Programs use uniquely named native device shader-list entries, exact byte matching, native Valid/create/reset handling, and the existing GxDevDestroy cleanup seam through DestroyPrograms. There is no per-draw creation/destruction, static saved device pointer, private COM ownership cache, framebuffer capture, raw address, binary hook or fake shader wrapper. Source recipes borrow no live source buffers or texture ownership.

## Limits and verification gates

Actual native program byte identity, source assets, sampler behavior, GPU appearance and gameplay are untested in this task. The exact source asset was not located as a loose fixture during the initial inspection; all source contracts and shader bytes come from the existing donor implementation/policies. The path is case/slash normalized but requires the .m2 filename. Multiple skins, material-layer combinations, material sharing with the third pass, arbitrary TXAC, source variants outside the exact triplets, other masked meshes, refraction, DX11/OpenGL and unsupported native shader permutations remain rejected or suppressed. This does not implement the donor omission diagnostic or general masked-material trial.

Parent gates are the combined full client build/link; exact source/build binding; loading the actual matching model/textures; both custom passes plus unchanged third pass; all four kernel modes and mode0 rollback; runtime/native program identity evidence; camera/animation/death-effect comparison; device loss/reset; interleaved native/custom/particle draws; texture-stage restoration; unload/reload; D3D9Ex and negative backend/source cases. Build success alone establishes neither visual acceptance nor full retail material support. Installation, accepted state, packaging and publication remain separate.

Rollback before integration means leave this stage unused. After integration, reverse only reviewed candidate hunks/new files and preserve unrelated dirty work; do not reset or clean the checkout.
