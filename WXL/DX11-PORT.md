# Deferred: DX11 is outside the current x64 WXL integration scope

The user explicitly deferred this renderer migration. The notes below are reference only.

# Native DX11 source port boundary

This checkout has native D3D9, D3D9Ex and OpenGL backends. DX11 has **not** been enabled or represented by a placeholder backend. Existing material blend7 and particle Screen functionality uses those existing backends.

The reference is `E:/WXL1.1/build/waystone-x64-reference/wowdecomp15595`, with actual DX11 implementation under `Engine/Source/Gx/CGxDeviceD3d11`. The backend comprises five `.cpp` files and one header (approximately 175 KB). Its device, resource, shader and state implementations depend on Cataclysm shared Gx contracts.

## Dependencies that prevent a backend-only transplant

- **Shader ownership and bytecode.** WotLK `CGxShader` owns individual `m_code`, validity and API objects; `CGxShaderPermute` is an unused placeholder. Cataclysm `CGxShader` owns a compressed block container and an array of real permutations. Its `CGxShaderPermute::Load` decompresses selected permutations and the DX11 backend consumes DXBC. WotLK D3D9 shader bytecode cannot be passed to `ID3D11Device::CreateVertexShader` or `CreatePixelShader`.
- **Shader callers.** WotLK `CShaderEffect::Init` creates arrays using `ShaderCreate(array,target,path,name,count)`, then binds `CGxShader*` entries. UI code in `Frame/CSimpleRender.cpp` does the same. Cataclysm uses `ShaderCreate(target,name)`, permutation objects and a shader model/profile selector. A compatible migration needs real WotLK-call adapters and verified shader constant, permutation, texture and input-layout contracts. Matching names alone do not establish this.
- **Framebuffer ownership.** The reference expects `CGxFrameBuffer`, `m_frameBufferVersion`, backbuffer color/depth textures and `ICreateBackBufferFB`. The current WotLK base instead maintains direct render-target/depth-surface behavior. Replacing it affects the native shadow and postprocess callers and requires explicit parity work.
- **Base device APIs.** The reference changes `ScenePresent()` to `ScenePresent(void*)`, adds draw-by-reference and instancing methods, changes query return/output argument types and source/destination texture-copy signatures. It also expects shared window lifecycle functions and resource lists. Copying the derived class leaves required WotLK virtual methods unimplemented.
- **Formats and capabilities.** The reference adds texture formats, texture targets, shader profile arrays, multiple render targets, multi-stream vertex layouts, monitor metadata and shader model fields. Numeric values must be translated deliberately rather than replacing `Types.h`, which would invalidate existing WotLK tables.
- **Construction and build.** Only after those contracts exist should `CreateCGxDeviceD3d11`, requested-API selection, adapter enumeration and Windows-only backend CMake sources be connected. DXGI/D3D11 are loaded dynamically by the reference implementation; extra link libraries alone do not supply missing shared source contracts.

## Required coherent implementation sequence

1. Add the container/permutation loader behind the existing WotLK shader API, preserving D3D9 ownership and callers. Bounds-check every compressed block and permutation. Select backend-compatible code explicitly.
2. Supply an exact shader asset family for the WotLK material/UI/terrain/effect semantics, either generated DXBC or a real shader translator. Keep DX11 unavailable if this family is absent; do not substitute a success-returning stub.
3. Adapt resource, framebuffer, caps and draw APIs additively while retaining native WotLK behavior. Port the reference device/state/texture code only after each dependency has an actual implementation.
4. Connect device construction, adapter enumeration and CMake, then build and verify device reset, UI, models, particles, shadows, fog, weather, liquids and postprocessing in the same asset/settings family.

No whole shared-base replacement was performed in this batch. Native fog, weather, liquids and postprocess remain owned by the WotLK source. Build, runtime and visual acceptance are still separate gates.

## Implemented particle extension subset

`AssetCompat.cpp` now reads chunk-relative EXP2 records before native relocation. A supported record has finite constant RGB/alpha multipliers in [0,16], zero extra scalar and an absent or constant-zero cutoff. TXAC, when present alongside EXP2, must have its exact material-plus-emitter row count and neutral per-particle entries. Constant EXPT RGB/alpha multipliers are now supported by the separate particle module; nonzero zSource and layered/cutoff semantics remain unsupported.

The converter retains source emitter ordering, and the adapter requires the source/target count to match. Non-unity multipliers create private, aligned native color/alpha key arrays; aliased original arrays are never modified. Existing native interpolation and rendering read those arrays. Color values must remain in [0,255] and alpha in the nonnegative native fixed16 range; saturation/HDR behavior is refused rather than approximated. Alpha baking quantizes back to fixed16. Runtime palette replacement can override RGB tracks and therefore does not establish EXP2 multiplier parity for palette-driven particles.

This does not implement TXAC UV composition, layered particle shaders, animated cutoff, nonzero EXPT zSource or general modern particle parity. Models without EXP2 keep the existing single-texture compatibility behavior. Unsupported extension records log a specific refusal before replacing the original model buffer. Disabling particle compatibility keeps the existing emitter omission policy.
