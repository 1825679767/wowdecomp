# Explicit native donor SKIN compatibility policy

Source-only stage against native HEAD6b1838b392fa722f0c07475e65fdecde407575ff. The captured source status/diff were empty. Complete changed/new files are under review/, exact prior files under before/. No active source writes, compilation, tests, game launch, installation, or release promotion were performed.

## Concrete integration

`AssetCompat.cpp::ReadCompanion` first runs the existing source21/33/35, DnD and exact Rift preparation. Any prepared SKIN bypasses this policy. Otherwise a per-model snapshot of the default-ON `wxlDonorMaterialFallback` control enables `PrepareDonorMaterialSkin`. Its owned clone passes through the existing `ConvertForNativeRuntime` model validation and `ConvertSkin` byte conversion; final UV1 adjustments are verified and consumed in the actual model/SKIN publication loop. `PrepareSkin` continues comparing the exact original archive SKIN before publishing the paired converted SKIN.

Shared/vendor M2Bytes, Convert, service/partition policies and Win32 source are unchanged. This is an explicit native loading adapter, not relaxed source admission in a shared converter.

Build option: `WXL_ENABLE_DONOR_MATERIAL_FALLBACK=ON` (private WxlM2 compile definition). Runtime control: `wxlDonorMaterialFallback`, default1 when compiled; disabled builds cannot enable it with the CVar. Reload models after changing. Independent source-owned exact recipes remain active when this approximation is disabled.

## Ported donor rules

Reference: E:/WXL1.1/extensions/wxl-modern-m2/src/compat/Skin.cpp::DownConvertBatch / EnvSplit / NextLookup. Original packed forms already admitted by the strict converter are left alone.

- Source named effects0..2 with more than two real source stages keep the donor's two-stage native effect; exact two-stage effects remain on the existing strict path.
- Donor low-byte cases5/8/10/12/16/23 collapse to one diffuse stage, including aliases above0x80ff. Previously supported exact cases remain unchanged.
- Other high named IDs use the donor default `0x0010`, one diffuse stage. This explicitly discards named retail equations/additional layers; it is logged as donor approximation.
- Environment low-byte cases0/3/9/17/24 split into donor primary(1,4) UV(0,-1), plus material+1/layer1 single follower. Case2 uses primary(1,3), and requires exactly two source stages because the donor does not clamp that case's primary count. Existing exact cases retain strict conversion.
- Low-byte cases1/15 outside the existing exact native named-effect path split into primary diffuse UV0 and material+1/layer1 follower using textureCombo+1 and actual secondary UV1. Weight/transform lookups advance only when their second entry exists, matching NextLookup. The clone temporarily carries the follower through strict single-stage serialization; Finish verifies the converted batch and the exact native six-entry UV lookup, then selects the existing real UV1 entry. No synthetic vertex UV or GPU feature is introduced.
- Low-byte21/33/35 never fall through to diffuse or fake ModMod. Missing existing exact renderer preparation remains an explicit refusal. Existing prepared DnD/Rift skins are never overridden.

All original referenced source texture stages are bounded and checked before dropping any layer. Split followers require a real next material and source second texture. Compatibility is bounded to16384 source batches and64MiB per file/output. All existing strict bone palette/influence, geometry, section, retained track, FDID and animation validation still runs. Unsupported packed forms below0x8000 remain refused.

## Ownership and evidence

Each affected SKIN owns an immutable recipe recording original24-byte source batches, source shader IDs/counts, rule, first native output batch and fingerprints. Exact original archive SKIN bytes remain model-owned. When any fallback is used, the original complete raw model is also retained until that CM2Shared's ModelAssets is released. No source/runtime pointer borrow escapes.

Logs identify each affected model/SKIN, source fingerprints, per-rule counts and the first16 source batch IDs/shaders; all remaining raw records stay owned in the recipe. Logs explicitly state that retail shader parity is not provided. Full matching source bytes and integration patch are auditable in this stage.

## Remaining bounded refusals

This policy does not add retail named shader equations, new texture stages, native source21/33/35 capability outside existing recipes, bone/palette/UV capabilities, unresolved texture IDs, unsupported particles, malformed source data, missing split followers, or source case2 with more than two stages. Runtime appearance and visual acceptance remain open.

## Root merge

Copy the new DonorMaterialFallback.hpp and modules/m2/DonorMaterialFallback.cpp. Merge AssetCompat.cpp changes around ReadCompanion and its paired conversion loop. CMake adds one option/source/private definition; NativeFeatures.cpp adds one include/register call. Preserve root's concurrent body-material/Rift additions when applying these existing-file hunks.
