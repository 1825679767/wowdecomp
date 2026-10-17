# C-test archive detection startup fix

User's7c9dd34 launcher reports Failed to open archive interface.MPQ. C:/Eunoia Wxl Mpq/Data contains the actual Wrath common.MPQ/common-2.MPQ and enUS locale archives, not the alternate split interface/model/texture layout. No archive rename, dummy archive or copied client is appropriate.

Root cause in CommonMain: SetPaths ran before InitializeGlobal registered and parsed command-line arguments. CMD_DATA_DIR was empty when SetPaths consumed it, so it reset the working/base directory from C-test to the immutable executable output folder. CheckDataFilesExist then found no common.MPQ and selected the split layout. This is an executable startup-order bug; the previously supplied -datadir launcher was not yet proven working.

Both argument families are now registered/parsed exactly once in CommonMain before SetPaths and startup logging. InitializeGlobal retains LoadMFIL and the remaining initialization order. Existing default-to-executable-directory behavior without -datadir remains. Native archive signatures, required members, priorities, locale selection and lettered patches remain unchanged. No missing-archive error is suppressed.

The combined candidate retains7c9dd34 donor female-Orc eye finalization, custom armor shader0x4011/opaque0x4001 draw identity, custom batch ownership through optional layered optimization and bounded absent lookup handling. This startup fix does not establish Human eye, retail gear or race visual acceptance.

Build/source binding, one owned no-account startup observation if possible and exact C-test launcher receipt are separate evidence. Leave E-live unchanged, retain prior launcher/runtime, FPS optional, no full client/Data copies, no game credentials or account actions. Broad creation/race/gear/world/save/50Hz/uncapped-memory acceptance remains open.
