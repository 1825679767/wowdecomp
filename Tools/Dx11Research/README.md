# Native WotLK x64 D3D11 backend

This branch adds an opt-in native `CGxDeviceD3d11` implementation to the Wrath Gx contract. It does not inherit D3D9 API state or replace shared game layouts. The ordinary D3D9 build stays the default. Select `SET gxApi "D3D11"` in a separate diagnostic client's WTF/Config.wtf after building with the option below.

## Build

Follow [DEPENDENCIES.md](DEPENDENCIES.md) to fetch the exact hash-verified Microsoft packages. Configure a fresh x64 build directory:

```powershell
cmake -S <source> -B <build> -G "Visual Studio 18 2026" -A x64 -DWXL_ENABLE_DX11=ON -DWXL_DX11_WDK_ROOT=<dependencies>/wdk/c/Include/10.0.26100.0 -DWXL_DX11_SIGNER_DLL=<dependencies>/dxbcsigner/build/native/bin/x64/dxbcSigner.dll -DWOWCLIENT_OUTPUT_OVERRIDE=WowClient64-DX11
cmake --build <build> --target WowClient XvidDecoder NativeDx11BackendTests --config Release --parallel 6
```

Use the existing source-owned signature-check configuration appropriate to the intended data; this backend does not change that policy. Keep the executable, matching XvidDecoder.dll, official AMD64 dxbcSigner.dll and its license together. Keep all Win32 proxy/core/water DLLs out of this family. No D3D12 device or translation mapping layer is linked.

The shader converter consumes the actual mounted Wrath shader bytes. Stock profiles 1.1 through 3.0 remain authoritative; Cataclysm DXBC with matching names is not a substitute. Missing/zero-byte profile placeholders permit fallback. Nonempty malformed families, wrong stage and incorrect permutation counts fail closed. Reload failure preserves the existing code. Pixel variants are keyed to their actual paired vertex output signature, layout and raster state.

## Implementation

- Native hardware/WARP feature-level-11 device, swapchain, fullscreen/window mode, resize, MSAA, gamma save/restore, software cursor and frame-lag fence.
- Native CPU-owned buffer pools and texture callbacks, mip/face/partial uploads, format conversion (including sub-block compressed textures through the native CPU blitter), generated mips, render/depth targets, target save/restore, rectangle/scaled copies, float color and multisampled depth readback, occlusion queries and device recreation.
- Exact native vertex layouts and typed/BGRA inputs, indexed/nonindexed primitives and triangle fans, native blend equations/write masks, culling/depth/scissor/viewport/sampler state, alpha rejection, clipping and native shader constants.
- Microsoft MIT D3D9 shader conversion to SM4 DXBC, signed with Microsoft's official signer. Generated fixed-function adapters cover native texture cascades, texgen/transforms, material/color sources, four directional/point lights, specular and fog.
- Bounded transactional GXSH v3 and independent compressed GXSH v4 readers. v4 support is a research/parser facility; the native game loader deliberately uses its own v3 shader family.

The native CPU DXT fallback retains the existing blitter color expansion (for example, a maximum RGB565 red becomes 248); ordinary hardware BC sampling retains its standard 255 result. Tiny or generated-mip compressed resources use that existing native CPU behavior.

Stereo and the three undocumented extension slots have the same unavailable/inert boundary exposed by the ordinary native contract. No stereo capability is advertised. D32 polygon bias uses D3D11 floating-depth precision and still needs visual comparison; the normal D24 path maps native normalized bias to fixed-depth units.

## Validation and remaining acceptance

`Tools/Dx11Research` is also a standalone CMake project for the v3/v4 parser tests, synthetic WARP proof, inspector and real-shader audit. Build it with the same dependency options, then run CTest. `extract_shader_corpus.py` opens the explicitly supplied MPQ fixture read-only, records exact archive/member hashes and extracts every available profile. The audit converts each nonempty permutation and calls actual D3D11 shader creation.

Run `<build>/bin/Release/NativeDx11BackendTests.exe <native-UI-vs_3_0.bls> <native-UI-ps_2_0.bls>` for WARP, adding `hardware` for the adapter or `window` for the hardware swapchain create/present/resize/recovery checks. The test retains fresh disposable shader-file fixtures under its working directory. It exercises mounted SFile family caching/counts/reload, real stock UI permutations and texture/indexed pixels, mixed fixed/native shaders, directional lighting, alpha/blends/write masks, cube/float targets, copy/stretch, queries, recovery and three MSAA depth formats. It initializes the native Storm and time-manager prerequisites.

Compilation, API creation and these pixel tests do not establish game/world or visual parity. Require actual login/UI/character/model/terrain/WMO/particle/liquid/shadow/postprocess rendering, reset/focus/fullscreen/streaming and performance comparisons before acceptance. Use a fresh local fixture, preserve data and accounts, and retain the original D3D9 launch path.

The combined backend also has native WXL adapters for scoped mesh/particle
materials, blood scene refraction, reaction-colored unit silhouettes, WMO overlay
and four-layer height/tint materials, detail-doodad grass motion and quest/corpse
navigation quads. The graphics checks exercise real GPU pixels, scene depth,
state restoration, alpha coverage and recreation. Grass checks preserve native
point lights and compare disabled motion against all twelve exact stock shader
permutations before checking actual displacement. These are implementation and
regression proofs; in-world appearance and representative performance remain
separate acceptance gates.

For the reported Ogre companion failure, run
`NativeDx11BackendTests.exe --m2-external <extracted-ogre-fixture>` against the
retained two model files and their `animations/` companions. This invokes native
relocation and interpolation over the declared sequences, aliases and channels.
For a new runtime reproduction, set `WXL_M2_EXTERNAL_TRACE=1` only in the private
test launch environment. A bounded, flushed per-process log under `Logs/` records
the actual model and external `.anim` filename before loading and relocation.
Passing extracted-byte checks does not establish that the reported race crash
is fixed; retain the exact runtime failure and matching executable/symbols.

## Import into the active x64 branch

The separate DX11 track starts at `760079926aada4cf4e46c4cc3d83a89f3d4f0907`. Apply its ordered commit series to an isolated integration branch, retaining all subsequent WXL modules and game changes. Review overlapping root/Gx/ThirdParty/Wow CMake changes and Gx factory/adapter hooks; do not replace whole files with older copies. The shader translator and new device are additive. Build both DX11 ON and the default OFF configuration, then repeat native tests and game acceptance against the combined source. Bring `fetch_dependencies.py`, licenses and the signer deployment hook with the code. Release-control pending intake is not promotion or permission to publish.
