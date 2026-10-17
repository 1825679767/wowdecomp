# Latest upstream wowdecomp main integration - 2026-10-17

Fetched official SatyPardus/wowdecomp main b1c71b8bd108823bca952f16c6f83a7ad33b9648,52 upstream graph commits beyond prior base cd29da38b9162419ee0fa29c6b16e07e10f6c4f0,129 changed paths. Three-way merged into native WXL branch after5e6e83bdc354a5b3c0a81376a359bf94123ed6fd; overlap review preserved current WXL and DX11 ownership. WXL module implementation paths have no upstream delta. Most changes restore named/typed constants without changing numeric values; they are not52 independent bug fixes.

Functional fixes include Win32/Win64 relative-mode cursor anchor and refresh/failure handling (5b41f72), console height initialization (eb5e655), guild-bank packet spell-charge byte width (1ba4a32), Unit transport override (f48c86f), and integer Lua attack power values with final native CMath::fint_ conversion (56bb138/56888bd). Includes final upstream archive-initialization behavior correction8bf90bc rather than an intermediate version. No assertion these fix untested WXL appearance issues.

Current retail model/atlas/3D item/private appearance fixes, female Tuskarr/Murloc routes, Broken palette/Hide Gear, central form control, load-ready choreography, native-datadir archive routing, roster31 and optional FPS controls are retained. Original dirty Win32 donor and separate DX11 research tracks untouched.

Validation: one combined Release WowClient/XvidDecoder build, actual prebuild source manifest/diff and postbuild hash verification, current dirty12file donor binding. Any typed-API caller adjustments required by compilation must be explicit and source-bound. No tiny fixture loops, new client/Data/Downloads copies, archives/UI/config edits, account actions or gameplay acceptance. C-test launcher only with rollback; E-live unchanged. The new upstream behavior and inherited visual/world/reset/memory/persistence gates await user testing.

Upstream review evidence E:/WXL1.1/build/wowdecomp-upstream-review-20261017. Source refs distinguish current merged native commit and pinned upstream main; no publication/promotion.

Compatibility adjustment after the first merge build: WXL ItemVariants now passes upstream ITEMSOUND_DROP instead of literal1 to SI2::PlayItemSound. Same numeric operation; no equip-sound behavior change. Original3edc903 build failure and exact source/log binding retained in native-wxl-dx11-upstream-main-sync-20261017/FAILED.md. No output from that failed build was exported/installed.
