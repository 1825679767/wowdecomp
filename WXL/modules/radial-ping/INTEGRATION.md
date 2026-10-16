# Native radial-ping gameplay module

This source-only stage replaces the radial-ping portion of the raw legacy packet
facade with a compiled, typed native module. It includes real source world picking,
unit tracking, projection, bounded server packet ownership and the donor C_Ping
bootstrap. It does not replace native rendering, fog, liquids, weather or graphics
backends. No tracked checkout edits, builds, tests, account/network execution,
Data copies, Downloads or commits occurred here.

Root integration:

1. Compile `WXL/modules/radial-ping/RadialPing.cpp` as C++20, with existing WXL and
   repository include roots and the native protocol/runtime/FrameScript/object/UI
   modules. No external dependency is added.
2. Add default-ON `RadialPingEnabled()` to NativeFeatures, honoring the master
   native/protocol/client-extension gates. Donor configuration is
   `WXL_RADIAL_PING=1`; use the existing feature registration convention.
3. Apply the staged ClientHelpers lifecycle calls. `Install` runs before the
   facade; `UiReady` installs its deferred notification pump after FrameXML;
   `Destroy` clears pending state before UI context destruction.
4. Apply the explicit staged LegacyPackets allowlist removal of outgoing `0x521`
   and incoming `0x522`. They belong exclusively to this compiled owner and must
   not also be registered by the facade. Atomic native registration commits both
   directions together. The native transport hides those outbound IDs from raw
   legacy sends and routes replies directly to the owner, outside raw-pop queues.

The Lua surface matches the existing donor:

- `wxlwow.radial_ping.pick_cursor()`
- `wxlwow.radial_ping.world_to_screen(x,y,z)`
- `wxlwow.radial_ping.unit_position(guidLow,guidHigh)`
- `wxlwow.radial_ping.send_server(type,x,y,z,guidLow,guidHigh,unitName)`
- `wxlwow.radial_ping.pop_server()`

Native registration uses the same `_WXLWOW_RADIAL_*` names consumed by the donor
bootstrap. The existing RadialPing addon remains responsible for its radial menu
and icon/frame presentation. This module now supplies actual native world data
and server delivery instead of requiring that addon to fabricate them from the
generic packet facade. `C_Ping`, `C_PingSecure` and enum compatibility bootstrap
is extracted verbatim from the donor domain module.

World picking uses the source `CGWorldFrame::HitTestPoint` with the existing
native cursor coordinate conversion. Projection uses source
`CGWorldFrame::GetScreenCoordinates`, near/visibility checks and native DDC
conversion. Unit attachments are published only when the GUID resolves through
the current native object manager as a unit/player. Unit tracking calls the
typed virtual `GetPosition` and `GetNamePosition`, with bounded/finite overhead
validation. There are no executable offsets, raw unit-field reads, injected
hosts or guessed object-manager layouts.

The precise donor request is one type byte, three doubles, two uint32 GUID halves
and a uint32-length name (37 fixed bytes). Response adds a second uint32-length
sender name (41 fixed bytes). Each name is at most 64 bytes; responses must match
the exact payload length, known type 0-3 and finite representable coordinates.
Outgoing names preserve donor control/colon sanitation. Server replies enter a
32-record queue; no gameplay authority or acknowledgement is synthesized.

Receive callbacks run only pure native parsing/state operations under transport
ownership. Lua notification is deferred to a root-UI OnUpdate pump after every
lock is released. The pump calls the existing
`wxlwow.radial_ping._NativeChanged` hook. Session reset, world absence, feature
disable and map change clear pending records. Lua pushes use trivial copied
record storage, with no owning C++ object or mutex crossing Lua allocation.

Quest-marker, challenge-mode and skyriding donor contracts were inspected to pick
this bounded slice. Their distinct D3D marker geometry, private game-object/model
selection and movement/input ownership remain separate work; this stage does not
claim those modules were ported. Native compilation and actual addon/world/server
behavior remain separate gates.

Donor:
`C:/Users/blanc/.codex/worktrees/x64-waystone/WXL1.1/extensions/wxl-radial-ping/src/RadialPing.cpp`.
Copyright/license notices are retained in the adapted C++ and extracted Lua.
