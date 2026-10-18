# wowdecomp-dx11: native x64 DX11 + WXL development source

This fork shares our current Windows x64 Direct3D 11 work on top of [SatyPardus/wowdecomp](https://github.com/SatyPardus/wowdecomp), a C++ reconstruction of the World of Warcraft 3.3.5a (12340) client. It contains the **combined native WXL integration**, not just an isolated graphics backend. It is intended for developers and their coding agents to inspect, build and integrate.

`main` is the shared development snapshot. `codex/wxl-native-x64` exposes the same source at publication. The original upstream history is preserved; the last integrated upstream point is `b1c71b8bd108823bca952f16c6f83a7ad33b9648`.

The latest compiled/tested code checkpoint is **`69b5ba38be507fcd41c2877c399d5748fee0f8d0`** (October 18, 2026). Later documentation commits do not imply another client build. This is experimental development source; rendered parity, performance and gameplay acceptance are separate from compilation and automated checks.

Start with the [developer and agent handoff](docs/DX11-DEVELOPER-HANDOFF.md), [backend notes](Tools/Dx11Research/README.md) and [WXL module map](WXL/README.md).

## What is included

- Native `CGxDeviceD3d11` backend, legacy Wrath shader conversion, resource/state ownership, GPU frame retirement, recovery and offscreen regression tests.
- Native WXL model/material/particle, appearance/equipment, world-data and client protocol modules, including their current integration fixes.
- Default D3D9/D3D9Ex/OpenGL paths. DX11 is additive and remains a build-time opt-in.
- Source and existing third-party notices. Game assets, MPQs, user configuration, accounts, build outputs and the official signer binary are not supplied by this work.

## Build DX11 on Windows x64

Use Windows 10/11, Git, **Python 3.11+**, CMake and Visual Studio's Desktop development with C++ workload with a Windows SDK. The current combined build was verified with **Visual Studio 2026 (generator `Visual Studio 18 2026`)**. The upstream project documents Visual Studio 2022 too; this fork's current combined checkpoint has not been revalidated with that toolchain.

From a Developer PowerShell:

```powershell
git clone --branch codex/wxl-native-x64 https://github.com/Furioz420/wowdecomp-dx11.git
cd wowdecomp-dx11

python Tools/Dx11Research/fetch_dependencies.py --output .dx11-dependencies
$deps = (Resolve-Path .dx11-dependencies).Path

cmake -S . -B build/dx11 -G "Visual Studio 18 2026" -A x64 `
  -DWXL_ENABLE_DX11=ON `
  -DWXL_DX11_WDK_ROOT="$deps/wdk/c/Include/10.0.26100.0" `
  -DWXL_DX11_SIGNER_DLL="$deps/dxbcsigner/build/native/bin/x64/dxbcSigner.dll" `
  -DWOWDECOMP_DISABLE_SIGNATURE_CHECKS=ON

cmake --build build/dx11 --config Release `
  --target WowClient XvidDecoder NativeDx11BackendTests --parallel 4
```

The dependency fetcher verifies pinned official Microsoft packages before extraction. See [dependency details and licenses](Tools/Dx11Research/DEPENDENCIES.md). It does not install a client. Signature checks are disabled above to match development with modified assets; omit that option to retain upstream's default signature checks.

The client target writes `bin/Win/WowClient64.exe`; keep its matching `XvidDecoder.dll` and `dxbcSigner.dll` beside it. The native test executable is under `build/dx11/bin/Release/`, with its own matching dependencies copied by CMake. Win32 injected/proxy DLLs are not dependencies of this native x64 family.

WXL feature options default mostly ON in this branch. `WXL_ENABLE_NATIVE=OFF` suppresses native WXL feature behavior but **does not remove its linked modules or all backend references**. For a standalone DX11 port, review the [integration boundaries](docs/DX11-DEVELOPER-HANDOFF.md#integration-boundaries) rather than deleting `WXL/`.

## Run with an existing client

Use your own compatible 3.3.5a client data. The native `-datadir` option selects the existing client root, so no Data copy is needed:

```powershell
$exe = (Resolve-Path bin/Win/WowClient64.exe).Path
& $exe -datadir 'C:\Path\To\ExistingClient' -d3d11
```

The same build accepts `-d3d` for the D3D9 path. Use the matching data/addons/server when exercising WXL custom features; enabling DX11 does not create retail assets or server protocol support. Logs and saved settings are written under the selected data root, so use a client you intend to test with.

## Focused renderer checks

Run from a disposable working directory outside your client. These modes open no game/account session and require no client Data:

```powershell
$tests = (Resolve-Path build/dx11/bin/Release/NativeDx11BackendTests.exe).Path
& $tests --presentation-apply warp
& $tests --frame-resources warp
```

`--presentation-apply` passed **11 checks** at the latest code checkpoint: VSync on/off and a windowed refresh selection preserve target contents; actual resize remains usable. `--frame-resources` is a separate bounded uncapped-resource regression; its presence is not a claim that every game scene has bounded memory. See the [backend README](Tools/Dx11Research/README.md) for shader-fixture, hardware/window and model-companion tests.

## Current limits and open checks

- In-game VSync Apply previously caused severe unresponsiveness. The latest checkpoint avoids a full window/target reset for presentation-only changes. Offscreen regression passes; the reported hardware-window scenario still needs confirmation.
- NPC Human guards had a separate model/baked-texture mismatch. The latest checkpoint routes matching NPC families to the canonical Retail model, preserving packed baked textures and native model-less helmet policy. Actual guard visuals remain pending.
- Earlier reports include retail race eyes/materials, equipment collections, loading latency, graphics settings and uncapped performance/memory behavior. Do not infer complete x32 parity from the source being present.
- Some modern materials use explicitly scoped donor approximations. Unsupported shader/positioner semantics are not claimed as exact retail behavior.
- FPS limiting is optional. The backend's bounded GPU work queue is independent of a user FPS cap.

## Source map

| Path | Purpose |
| --- | --- |
| `Engine/Source/Gx/CGxDeviceD3d11/` | Native DX11 device, buffers, textures, state, draw and presentation |
| `Engine/Source/Gx/ShaderContainer/` | Wrath shader containers and translation |
| `ThirdParty/D3D9ShaderConverter/` | Pinned Microsoft converter subset and notices; no D3D12 device layer |
| `Tools/Dx11Research/` | Dependency fetcher, parser/renderer tests and extraction tools |
| `WXL/` | Native WXL integration, public interfaces and vendored provenance |
| `WoW/Source/`, `WoW/Common/` | Game, UI, character and protocol integration sites |
| `docs/DX11-DEVELOPER-HANDOFF.md` | Entry point for a developer or coding agent |

## Upstream and attribution

The original client reconstruction retains `// OFFSET:` annotations for comparison with build 12340. Upstream's function/code coverage statistics describe reconstruction coverage, not this fork's visual or gameplay acceptance. See [upstream wowdecomp](https://github.com/SatyPardus/wowdecomp) and its README for its own progress and Win32 workflow.

Preserve the existing notices and provenance in WXL/vendor and ThirdParty. The shader converter's MIT notice is in `ThirdParty/D3D9ShaderConverter/LICENSE`; fetched packages retain their own licenses. This fork does not assign a new blanket license to third-party or upstream code.

Upstream credits include binana, thunderbrew, whoa, TOMRUS, bug reporters and the WoW modding community. Upstream community: https://discord.gg/DsZMGUD8aT.
