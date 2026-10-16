# Native custom opcode transport

This source implementation reuses `NetClient`, `ClientServices` and `CDataStore`
directly. It has no executable addresses, module loader, packet hooks or account
login calls. The native receive callback still checks its connection and
copies accepted data into the native event queue; `ProcessMessage` hands declared
custom opcodes to an owned queue on the FrameScript owner thread. All undeclared
opcodes follow the original path. Oversized declared packets are discarded before
the native event-queue copy.

`WXL_NativeSendCustom(opcode, binaryString)` returns whether a bounded payload was
handed to the existing native sender. It does not imply server acknowledgement.
Only compiled outbound declarations can send. `WXL_NativePopCustom()` returns
`opcode, binaryString`, or no values when no current-session record is available.
The packet body is preserved as bytes; feature owners must validate their own
wire grammars before applying state.

Built-in declarations are spell-charge snapshot `0x526` inbound, area-loot
`0x550` outbound / `0x551` inbound, and garrison `0x552` outbound / `0x553`
inbound, matching the WXL portable feature contracts. These generic functions do
not replace existing feature-specific Lua bindings or their parsing/state logic.
Additional compiled owners can use `protocol::RegisterOpcode`; standard opcode
IDs, duplicates and more than 64 directional declarations are rejected.

Payloads are capped at 10,235 bytes. The additional owned dispatch queue is capped
at 128 packets and 4 MiB. The native event queue remains native-owned. Incoming
buffer bytes are copied; deferred consumers do not retain packet-buffer pointers.
Send and pop require the initialized FrameScript thread, an existing connected
current session and an unsuspended transport. Queued records carry client,
transport and reset generation. Disconnect, connection establishment and Lua
context teardown/recreation invalidate pending records. Saved owners are compared
without being dereferenced. Returned Lua strings are copied by Lua.

`ProtocolEnabled()` gates captures, sends and dispatch. It must be provided by
`NativeFeatures`, and `NativeProtocol.cpp` must be added to the WXL CMake sources.
Turning the feature off leaves standard protocol handling intact. Lua entry points
remain registered so UI code can observe false/empty results while disabled.

Appearance protocol version 2 uses hello/prepare/query requests (`0x540`, `0x542`,
`0x543`) and hello/snapshot/prepared replies (`0x541`, `0x544`, `0x545`). Its donor
also owns catalog negotiation, selection validation, prepare result handling,
character-create/barber coordination and snapshot application. Those dependencies
are not implemented here, and appearance save opcodes are not declared merely by
having a working transport.

Source references reviewed: `src/platform/waystone/Network.cpp`,
`include/wxl/AppearanceProtocol.hpp`,
`extensions/wxl-modern-m2/src/compat/ServerAppearance.cpp`, and area-loot/garrison
wire-contract READMEs in the x64-waystone source worktree.

This integration has not been built, tested, connected, installed or accepted.
