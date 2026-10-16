# Native client extension stage - 2026-10-16

Source-only staged integration. No shared source edits, builds, tests/probes, installation, publication, acceptance promotion or commits occurred. Root owns the combined compile and shared source integration.

## Files and build

Copy ClientExtensions.hpp, LegacyPackets.cpp, LegacyPacketsLua.hpp, ClientHelpers.cpp and ParagonCompat.cpp into one source-owned native module (for example WXL/modules/client_extensions). Add the three .cpp files to WxlClientExtensions object target and collect it once into WxlNative. Include the current source root, WXL, WXL/vendor/src and the staged module directory. C++20 is required. Add shell32.lib for CommandLineToArgvW; existing WxlRuntime supplies common/Log.cpp. No external Lua implementation is needed: uses existing FrameScript Lua5.1 headers and symbols.

ProtocolSeams.inc contains the two required public read-only transport queries and builtin-owner predicate. These do not declare opcodes or route packets. CanLegacySend is declaration-minus-private-outbound; CanLegacyObserve is declaration-minus-builtin/native inbound. Both reject stock IDs below NUM_MSG_TYPES. Keep the consumer owner predicate as its single owner list. Do not replace WXL_NativeSendCustom/WXL_NativePopCustom.

## Lifecycle seams

1. At the end of protocol::InitializeLua, after transport functions, builtin consumers and compiled appearance owners exist, call client_extensions::Install(). It registers native helper functions and legacy packet facade, then atomically registers paragon request0x554/state0x555 through RegisterNativeDispatch. Paragon registration failure leaves its Lua entry points absent. This stage does not register other gameplay wire IDs.
2. At protocol::UiReady after root FrameXML exists, call client_extensions::UiReady() to create the bounded OnUpdate reader dispatch pump. Each fresh Lua context gets a new pump; repeated UiReady is idempotent.
3. Before protocol::DestroyLua, call client_extensions::Destroy(). Source-owned writer and paragon queues clear. The transport remains responsible for its compiled owner registrations and generation resets. Do not reset or reuse writer IDs when reloading Lua.

## Legacy packet behavior

Preserves donor CreateWXLPacket(opcode,size), chained WriteUInt8/Int8/UInt16/Int16/UInt32/Int32/UInt64/Int64/Float/Double/String, Size, Send and OnWXLPacket(opcode,callback) with typed reader methods. Strings use uint32 little-endian byte count followed by exact binary bytes. Writer declared sizes retain the donor zero-padding/overwrite semantics. The opcode constant/alias catalog is preserved as names; naming a wire ID never declares it or grants send/observe access.

Actual serialization is typed C++. Integer inputs must be finite, integral and within range. 64-bit numeric operations are restricted to Lua double's exact integer range, +/-9007199254740991; unsafe full-width values refuse rather than silently round. Readers return nil on truncation, invalid numeric data or session mismatch and do not advance. Writers poison on invalid writes, preventing Send. Writer IsValid and lastSendSucceeded expose failures; Send retains donor chainability. Binary strings preserve embedded NULs.

Writers are bounded to128, payload10235, global8MiB quota and5-minute idle expiry. Numeric IDs never reset/reuse across Lua sessions, and writer storage clears on transport generation change. Each observer owns a separate data string/cursor; at most64 callbacks per opcode and128 packets per frame. Lua pcall isolates callbacks. Stored readers reject after session changes. The pump consumes only transport Pop output; native and builtin owners have already exclusively consumed their packets before that boundary. No outgoing packet bypasses the captured native Lua send entry point, including its nativeOutbound refusal and builtin payload validators.

## Client helper behavior

GetWXLClientExtensionsVersion returns legacy API version3. GetSpellDescription uses g_spellDB and native SpellParserParseText (same contract as trainer UI). WXL_HasArgument/WXL_GetArgument parse the actual Unicode Windows command line and return UTF-8. Mounted WXL_ReadClientFileText uses SFileOpenFile/size/read/close, caps files at8MiB, rejects embedded-NUL paths and preserves returned bytes. LOG_DEBUG/INFO/WARN/ERROR use existing common logging. No fixed addresses, x86 bindings, signature patches, Warden changes or archive writes.

## Paragon owner

Uses existing wire0x554 request and0x555 state and donor _WXLWOW_PARAGON_SEND/POP/CLEAR Lua functions. Request is1..64 printable ASCII bytes; state is1..8192 printable ASCII/newline bytes with nonempty lines<=512. Queue holds at most256 lines and drops oldest when full, matching donor. The complete packet is validated before enqueue; reset and generation changes clear state. Capture callback performs no Lua calls, sends or protocol reentry under the transport mutex. Native owner registration makes raw Lua send/observation of paragon private. No paragon gameplay rules or server changes.

## Limits and root review

No generic opcode-registration function is exposed to Lua. Existing addon owners must already register their raw-compatible IDs through source declarations; otherwise CreateWXLPacket/OnWXLPacket refuse. Stock opcode send/observation stays denied. Paragon is the only newly registered feature owner. Skyriding, challenges, weekly vault and other gameplay handlers are deliberately outside this stage. The module has not been compiled or exercised; root must resolve compile diagnostics and complete lifecycle/runtime acceptance separately.


Root integration adds default-ON WXL_ENABLE_CLIENT_EXTENSIONS/wxlClientExtensions,
WxlClientExtensions target and module discovery. The legacy transport whitelist
now explicitly registers the established donor raw-addon quest/radial/item/
skyriding/challenge/loot/toast/weekly/PvP IDs before installing the facade.
Naming a constant still grants nothing by itself. Builtin spell charges and
native appearance/paragon consumers remain exclusive; this whitelist does not
claim native skyriding/quest/challenge gameplay handlers are implemented.
Lifecycle calls are integrated at protocol init/UiReady/teardown. Native shader,
renderer and network authentication behavior is unchanged.
