# Native weighted AddAlpha mesh variants

E-only source candidate against captured HEAD6b8280048f3a88af104b5b704e96a57aa1d76365 and exact before-file hashes. The starting source already contained root's concurrent topology conversion changes, recorded in source-status.txt/source-dirty.patch. No active source writes, shader compilation, native build, tests, game launch, deployment, DX11 or Gx backend changes occurred.

## Actual integrated variants

The source21/33/35 custom material family previously required exactly one model bone, one section bone, source weights255/0/0/0, local palette0, and one SKIN profile. It now supports normal native GPU-skinned draws with1..4 real influences and bounded section palettes, plus1..4 individually owned named SKIN profiles. Existing DnD/Rift policy precedence and exact signatures remain unchanged.

The model caller passes actual CM2Shared cache flag8 (native GPU skinning ownership), sampled once into ModelAssets, to mesh_materials::Prepare. CPU-owned multi-bone preparation is refused. Every profile still owns a distinct immutable Source and selects that Source only when its matching archive SKIN is admitted by PrepareSkin. Prepared topology must remain unchanged; this stage does not grant root's separate partitioned topology capability to a stale material recipe.

Source admission validates every used section vertex window, lookup/property array bounds, palette size/range, every palette bone against actual model bone count,1..4 influences, contiguous nonzero source weights summing255, and each active property index against its section palette. No bone clamp, guessed global index, missing UV, implicit vertex duplication or geometry rewrite is used. Model bone count remains bounded4096; profiles1..4, batches/sections<=32 and source64MiB limits remain. Palette budget matches native model admission: min(75,(vertexConstants-(vertexConstants>192?64:32))/3), requiring256 hardware vertex constants for the weighted VS.

## Proven native GPU ABI

- M2Shared::SetVertices writes48-byte M2Vertex/GxVBF_PBNT2: normalized UBYTE4N weights, unnormalized UBYTE4 palette indices, position/normal/two UVs. Draw verifies this exact typed format/stride, active shared vertex buffer and D3D declaration mappings.
- CM2SceneRender::DrawBatch uploads three affine rows per palette bone at c31. The custom draw reasserts those same canonical model bone rows through ShaderConstantsLock/Unlock, including dirty-cache updates; it does not substitute external matrices or leave private constants behind.
- The weighted VS blends those native affine rows by the normalized byte weights, transforms both position and normal, then retains the donor's normalization, projection, UV transforms, lighting, fog, colour and grazing equations. Zero-weight lanes choose palette0 before dynamic addressing. The existing donor pixel shader bytes remain untouched.
- Original rigid source keeps its existing precompiled vertex shader and native rigid draw path. Its CPU owner may rebase bone combo offsets; the old live bone0 identity check is retained for that path.

## Program and state ownership

There is no donor multi-bone bytecode. The source variant is retained as sibling CombinersModAddAlphaSkinned.vs.hlsl plus its exact embedded C++ source. First use compiles vs_3_0 using a typed D3DCompile export loaded from an absolute GetSystemDirectoryW/d3dcompiler_47.dll path. The owned module handle outlives both compiler blobs and is released immediately after compilation. No load occurs under loader lock, no import-library dependency is added, and there is no injected hook or detached/global device cache.

Compiled bytes are bounded/target-checked and owned by the native device shader list, alongside the existing custom programs. An empty owned shader entry caches missing-compiler/compile refusal for the device lifetime, preventing a per-draw compile/load loop. Device reset retains code through existing native shader lifecycle; DestroyPrograms explicitly releases the weighted owner before device teardown. Missing compiler, failed compilation or unsupported declaration suppresses the custom family, with a bounded one-time compiler diagnostic. No stock-equation fallback is substituted.

Existing RsPush/RsPop plus typed D3D9/9Ex IStateSync restores shader/program state. Geometry, native UV buffers, textures, animation, palette ownership and draw ordering remain with native Model2.

## Unsupported variants remain explicit

D3D9/9Ex normal type0 GPU draws only. CPU-skinned multi-bone, OpenGL, insufficient vertex constants, projected type1 and instanced/doodad type2 weighted draws remain unsupported. Two narrow CM2SceneRender early gates suppress weighted type1/type2 routes that bypass the normal custom shader owner, preventing their prepared bytes from silently drawing a stock equation. No world/doodad rendering policy is widened.

The existing source21/33/35 material/texture/weight/transform contracts, donor shader equations, two UVs, and all downstream strict model/SKIN geometry validation remain. New retail equations, shadow/specular/point-light parity, topology partitioning of custom batches and a general renderer replacement are not claimed.

## Merge

Copy the two new shader source files and merge MeshMaterials.hpp/.cpp. AssetCompat.cpp contains only three narrow caller hunks: ModelAssets nativeGpuSkinning flag, its sampling from shared cache flag8, and the extra Prepare argument. Merge those into root's newer topology caller. CM2SceneRender.cpp adds AssetCompat include and two weighted-only early-return gates. Existing wxlCustomMaterials/default-ON module controls this implementation; no new toggle is required.
