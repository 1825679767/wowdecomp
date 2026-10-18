# DX11 developer and coding-agent handoff

## Source identity and intent

Shared repository: https://github.com/Furioz420/wowdecomp-dx11. `main` and `codex/wxl-native-x64` point to the publication snapshot. The full native WXL/DX11 branch history is retained. Upstream is https://github.com/SatyPardus/wowdecomp at the merged point `b1c71b8bd108823bca952f16c6f83a7ad33b9648`.

Latest compiled client/code source: `69b5ba38be507fcd41c2877c399d5748fee0f8d0`. Client SHA256: `5a949c90d266e87ab4c05c890ef1bbb678ad3858cfa76daddbbf6066961a7235`,11,857,920 bytes. The combined Release WowClient, XvidDecoder and native test target passed, with 3,925 tracked source identities and 12 scoped donor identities unchanged across compilation. Later documentation commits are not new binary source bindings. No client binary or game asset is included here.

This shares the actual current integration so another developer or agent can understand/adapt DX11 for their own changes. It is not a stripped renderer SDK or a claim of complete gameplay/x32 visual parity. Root build/run commands use portable paths and the existing client data root. Do not replay historical local build/install scripts or expect the original operator's paths to exist.

## Integration boundaries

1. Start at `Engine/Source/Gx/CGxDeviceD3d11/CGxDeviceD3d11.h`, `Internal.h`, `Device.cpp`, `Resources.cpp`, `State.cpp`, `Shaders.cpp` and `Copy.cpp`. These implement native device/presentation/resource ownership rather than a replacement d3d9.dll.
2. `Engine/Source/Gx/ShaderContainer/` and `ThirdParty/D3D9ShaderConverter/` translate the mounted Wrath shaders. Stock GXSH v3/SM1-3 input and its actual permutations remain authoritative. The independent v4 parser is not a reason to feed another expansion's shader assets into Wrath.
3. Follow root/Engine/Tools CMake and Gx factory, console/CLI, format/capability and shared native call-site changes. Enabling `WXL_ENABLE_DX11` alone is not the complete implementation delta. The [backend README](../Tools/Dx11Research/README.md) explains dependencies and interfaces.
4. The backend and native tests reference WXL-owned material, outline/refraction, WMO and teardown state. WXL is always linked by this branch; compile/CVar flags suppress behavior but do not establish a standalone link graph. Use [WXL's module map](../WXL/README.md) to find the actual owners. If extracting a pure backend, provide equivalent interfaces or carefully remove those callers and tests together.
5. `WXL/vendor/` retains scoped portable source and provenance manifests. Preserve its ownership and licenses. Do not replace it wholesale with another checkout or transplant x86 runtime offsets into native x64 structures.

## Latest fixes to inspect

- `69b5ba3` / `12ecff4`: VSync-only and windowed-refresh Apply update presentation without recreating the window/targets. Genuine target changes retain reset. A failed rendering context yields rather than tight-looping. The source preserves actual target dimensions despite the base Gx minimum-window clamp.
- `12ecff4`: NPC character-family routing follows the canonical Retail actor while preserving existing packed baked textures. Baked NPC equipment and model-less helmet behavior are separate from player atlas composition.
- `ead8cbe` / `5282f14`: Retail texture binding survives classic composition; full deferred atlases prepare all clothing regions; completed body/gear resources survive unrelated item-index publications.
- `992bffb` / `3edc903`: merge of upstream b1c71b8 and the WXL typed item-sound call adjustment.

These are useful source entry points, not a self-contained cherry-pick list; earlier device, shader, lifetime and WXL commits remain dependencies. Preserve history when comparing against your branch.

## Validation and known issues

The latest actual-backend WARP presentation regression passed 11 checks. It verifies target contents/settings across VSync on/off and windowed refresh selection, frame retirement, and a real resize. It opens no game account/session, client Data or visible window. The native test suite also has a bounded uncapped frame-resource mode and broader shader/pixel/target/recovery checks; follow each mode's fixture requirements.

Still open for representative in-game testing: the reported hardware-window Apply freeze, Human guard body/helmet appearance, retail race eyes/hair/materials/equipment collections, loading speed, uncapped memory/performance, world effects and server appearance persistence. Retain the prior known-working client and use your own intended test location. No default FPS limit is imposed; optional FPS CVars and GPU in-flight-work bounds serve different purposes.

Build and fixture success are not visual acceptance. When reporting a regression, include the exact executable commit/hash, renderer, arguments/data root, recent graphics/native logs and the action that triggered it. Source publication here does not publish an accepted runtime release or authorize a client-data redistribution.

## Collaborating

Keep your own feature branch off this snapshot, record its starting SHA, and integrate bounded commits. Upstream `origin/main` in your existing project may mean something different: in this sharing fork, `main` contains the combined work. Add SatyPardus/wowdecomp as a separate upstream remote when needed. Avoid force-pushing over another developer's work. Submit focused changes with reproduction and validation notes; do not claim fixes for untested race/world/material combinations.

Existing upstream, Microsoft converter, vendored WXL and package notices remain in force. No new blanket license is asserted. Assets, server credentials, user settings and downloaded dependencies belong outside Git.
