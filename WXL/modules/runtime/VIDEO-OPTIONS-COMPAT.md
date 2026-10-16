# Carried-over startup video Apply guard

The historical native build4 bridge had an exact-source repair for VideoOptionsPanels.xml/lua, but it was absent from the rebuilt source client. The source-native loaders now use that same pure-byte contract before XML parsing or Lua compilation. Canonical file paths, exact sizes, FNV fingerprints and expected6 XML/4 Lua replacement counts all remain required. Only VideoOptionsFrameApply:Enable() gains its existing nil guard. Unknown and already patched files stay unchanged. No substitute Apply object or global callback is fabricated.

Original source: C:/Users/blanc/.codex/worktrees/x64-waystone/WXL1.1/src/platform/waystone/VideoOptionsCompat.hpp
SHA256 5b0ef62c0bcc005c600b837a94725857f535723aec0a34f8d578ff65334ac77a

The native adaptation uses a C++17 pointer/size interface, ordinary temporary vector ownership through parse/compile, and still unloads the original SFile allocation exactly once. MD5, when requested, covers the actual parsed/executed bytes. Native archive, signature/authentication and UI application behavior are otherwise unchanged. XML's optional MD5 context is now checked consistently with the existing Lua loader. WXL compile-time disabling bypasses the repair.

This is source integration for the known startup issue. Prior historical fixture results are not rerun or relabeled as this source's runtime evidence. Current actual Gamma UI and startup/gameplay acceptance remain for the broad test. No MPQ or loose authority file writes.
