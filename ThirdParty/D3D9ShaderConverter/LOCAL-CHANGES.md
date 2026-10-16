# Local integration changes

- Added a standalone static-library CMake target using explicit pinned WDK include paths, without linking the D3D12 translation layer.
- `ShaderConv/context.cpp`: include `<algorithm>` and use `(std::min)` / `(std::max)` in the legacy pixel source-modifier clamp, preserving behavior with the native client's global `NOMINMAX` definition.
- `Inc/ShaderConv.h` and `ShaderConv/vsconv.cpp`: add the bounded `SwapRedBlue` input conversion for native packed BGRA vertex colours. Its three-bit conversion field retains the existing 16-bit packed declaration size. The native buffers remain authoritative; the translated vertex shader swaps channels in its input prologue. Integer and normalized-byte attributes retain their separate conversions.

The rest of the converter files are preserved from the MIT-licensed upstream commit in `UPSTREAM.json`. Native wrappers perform bounded instruction-span validation before calling the converter. Shader linkage, state-derived variants, API-object ownership and native constant buffers remain owned by the WXL Gx backend.
