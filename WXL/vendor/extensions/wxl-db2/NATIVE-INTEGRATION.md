# Native FDID reader subset

Donor: `extensions/wxl-db2` in the separate `x64-waystone/WXL1.1`
source worktree. Copyright and license notices are retained in each source
and in `LICENSE`.

The copied decoder and compact lookup implementation are unchanged.
`DB2File.cpp` uses compiled `wxl::native::ReadAsset` instead of the extension's
game-address storage wrapper, and checks that strings terminate inside their
owned blob. `ExtensionApi.hpp` is a native adapter using the shared log engine.
No plugin entry point, module service table, fixed address, or injector is used.

`WXL/FdidCompat.cpp` adapts the original resolver's shared FDID namespace and
stable node cache for forward texture/model lookups. It loads only
`DBFilesClient\\TextureFilePath.db2` and `DBFilesClient\\ModelFilePath.db2`.
Material, reverse lookup, Light/fog and retail item APIs are outside this subset.
The vendored decoder supports WDC1/WDC2/WDC3 (including 1SLC); unsupported table
formats fail resolution. No new WDBC/WDB2/WDC5 format interpretation is added.

Compile `WXL/FdidCompat.cpp`, `src/decode/DB2File.cpp`, and
`src/decode/Db2Decode.cpp` into the native executable, with C++20 and include
roots for `WXL` and the vendor root containing `common/Log.hpp`.
Link the existing shared `common/Log.cpp` implementation and the native
`ReadAsset` / `AssetsEnabled` implementations. No extra external library is needed.

Call only after the archive mount phase. The first enabled lookup initializes
the authority once; missing tables and misses remain cached for this process.
Disabled lookups do not initialize storage or populate caches. Returned paths
remain valid for the process lifetime. This source subset has not been built,
executed, installed, or accepted as a runtime candidate by this integration.
