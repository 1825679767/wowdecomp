# Native custom material CPU path

E-only source candidate against clean HEADfe93326bcec7dfe242a638b1ce681b61db0e8613. Three exact prior files are under before/, complete candidates under review/. No shared source writes, build, tests, runtime, DX11, world renderer, fog renderer, installation or publication occurred.

## Actual CPU integration

The source21/33/35 custom family now supports normal type0 multi-bone models owned by the native CPU cache (!CM2Cache flag8), with shader rendering enabled. Existing single-bone and native GPU-palette paths retain their existing programs and ownership.

- Source retains per-section original48-byte vertices, both UVs, source weights, palette-resolved global byte indices and exact section-local triangle indices. Existing source array bounds, normalized contiguous weights/influences, palette bounds, global bone bounds and strict model/SKIN validation remain. CPU global indices must fit the actual native uint8 contract (<=255), and at most65535 captured section vertices are retained per profile. Triangle indices must remain inside their actual section vertex window. CPU admission replaces the former blanket non-GPU multi-bone refusal with this concrete bounded owner.
- Draw calls the actual native M2BlendBoneMatrices helper against the current model's bone matrices. Position/normal transforms mirror TransformVerticesNoUVSelect_cpp exactly. The generated40-byte PNT2 stream preserves both original UVs; a native stream index buffer preserves triangle order with only section-local index rebasing. No source geometry is discarded or redirected.
- Native CPU matrices already produce view-space positions/normals. The existing precompiled donor rigid VS therefore uses scoped identity affine rows at c31..33. Projection, UV animation, colour, lighting, fog, grazing and the existing donor PS remain unchanged. No new shader compilation or CPU approximation of the pixel equation is introduced.
- The draw synchronizes and snapshots native state, then restores the complete vertex attribute/buffer mask and native format/buffer/stride caches, index binding, shader selection and c31..33 constants through dirty-cache-aware APIs and typed D3D9/9Ex IStateSync. Native stream pools retain temporary buffers; no borrowed actor/source pointer or independent global resource cache is introduced.

CPU geometry is source-owned per SKIN/profile, so it does not borrow the native !8 path's mutable globally-rebased model vertex arrays. Existing independent named profile selection remains authoritative.

## Concrete draw identity fix

M2Scene builds normal elements from CM2Shared::m_skinSections, a separate native runtime copy allocated by InitializeSkin. The old custom Draw guard compared that pointer to m_skinData->m_skinSections (serialized array), suppressing even valid GPU draws. This stage changes only that typed identity comparison to the actual runtime copy. The parent's concurrent DnD owner has the same separate comparison and was notified to correct its own file; DndMaterials.cpp is untouched here.

## Boundaries and missing contracts

CPU normal draws are unoptimized and unsorted (element flags1/4 remain refused); optimized geometry, depth-sorted index streams, shaders-disabled fixed function and CPU topology composition remain pending. Root's concurrent BindTopology must explicitly refuse cpuSkinning until it also remaps the newly owned CPU vertices/indices. No stale section/palette identity is granted by this stage.

Projected type1 rendering delegates to m_projectTextureCallback over receiver geometry, with EyeLinear UV/depth transforms, billboard view and projected shaders. The donor AddAlpha kernel instead consumes source mesh normals, independent original UVs and source-mesh grazing. There is no donor projected equation/receiver-normal contract in the inspected source; using either stock projected shaders or the ordinary mesh VS would change the effect. The stage keeps this path refused.

Native M2Scene chooses type2 doodad instancing only for nontransparent compatible batches. The custom family's required material blend7 makes transparent=true (blendMode>1), so these sources normally select type0. No real native-selected AddAlpha instancing contract is available to extend. Both projected/doodad guards now cover rigid as well as weighted AddAlpha Source, preventing a prepared custom batch from taking a stock equation if such a route is forced. Exact DnD Source is exempt from the new broadened guards, preserving its existing native third-pass behavior.

No new retail shader, projected/instanced parity, point-light/specular/shadow parity, palette widening or global renderer behavior is claimed.

## Root merge

Root concurrently owns MeshMaterials topology preparation/BindTopology and additional Source fields. Merge these hunks, not whole-file overwrite: SectionSource CPU ownership vectors; Source.cpuSkinning flag; Prepare CPU capture/admission; DrawCpu state/geometry helper; Draw CPU branch and runtime-section pointer correction. Keep root's topology fields/API and final device palette gates. CPU topology must remain explicitly refused for now. AssetCompat and vendor converters are untouched by this stage.

## Composed CPU topology and native geometry selection

The root integration now regenerates CPU-owned original-geometry streams from the validated rebuilt model/SKIN when palettes are partitioned. It resolves each active byte index through the final section's actual global bone palette, requires every used global index<=255, retains both UVs and the complete section-local triangle stream, and bounds aggregate CPU vertices per profile. Source batch/template identity validation remains unchanged. This supersedes the earlier CPU-topology refusal noted above.

Models with custom material owners now use ordinary native unmerged geometry after UnoptimizeVisibleGeometry retires any previous optimized buffers. This preserves the source batch/palette identity and native section visibility rather than letting the optimizer merge identities the custom shader cannot safely consume. Unrelated models keep native optimization.
