Merged native source integration. Combined compilation and gameplay acceptance remain separately recorded. Original staging contract follows.

# Native Skyriding source candidate

Pending source integration, unbuilt. This worker changed only this E-only staging directory. No shared source changes, builds, test runs, gameplay, installation or publication occurred. Root merges the shared wiring hunks and performs the combined build. The manifest records the actual observed source HEAD and exact input bytes, with no build-source binding until that combined build exists.

## Implemented source

The owned implementation is `candidate/WXL/modules/skyriding/Skyriding.cpp`, with C++17 public declarations/diagnostics in `candidate/WXL/Skyriding.hpp`. The private module uses C++20. The donor is `E:/WXL1.1/extensions/wxl-skyriding/src/Skyriding.cpp` plus its public diagnostics header. Donor phase/state constants, parsing, command interpretation, AdvFly sequence IDs, phase durations, hysteresis, double-jump timing, transient-mode behavior, and diagnostics were translated into typed native contracts. No injected ABI, hardcoded field offsets, absolute executable addresses or SEH pointer probing remain.

The implementation includes:

- Native exclusive opcode ownership: request `0x527`, response `0x528`, impulse ACK `0x529`, impulse `0x52A`. The text wire is `uint32 length` followed by exact command bytes (maximum 128). The impulse wire is exactly 24 bytes: mover GUID, sequence, three float directions. Its ACK is exactly mover GUID plus sequence (12 bytes).
- Server MODE/VERT/TURN/RATE/SPD/ANIM/LAND/GLOCK interpretation. CHG/AUX remain informational, as in the donor. MODE, ANIM and matched player impulses are the server-owned activation sources. A matching mounted model must expose sequence 1532 before animation ownership applies.
- Cruise/slow cruise/stall, dive entry/down/reverse exit, surge, full whirling sequence 1558 with fallback 1680, and the two-stage skyward flap sequence. The native all-bones animation seam leaves the owner's full phase/time/speed intact rather than repeatedly replacing its negative-speed exit animation with stock speed.
- Server impulse validation and exact ACK, retained impulse diagnostics, server-speed-based movement, skyward heading capture and eased pitch, turn inertia, and native queued forward coasting. Existing native flight permission/flying/root/transport/local-control state gates movement control. Native queued pitch/facing APIs apply native legality limits and ordinary status reporting.
- World-focused space/X handling, repeated-key handling, double-space TAKEOFF, classic-vertical bypass, BRK state changes, RESYNC on session/map/player/mount changes, and server-requested LAND/GLOCK flight exit through native movement events.
- Public Lua read-only `WXL_GetSkyridingState()` and bounded `WXL_SkyridingCommand(text)`. The named command API permits only the donor RESYNC, TAKEOFF and BRK commands, and requires the current native session/player/AdvFly mount. It cannot author server MODE/ANIM/rate/flight permission.

This ports native controller behavior into real gameplay owner seams; it does not establish gameplay parity or mount acceptance.

## Authority and retained limits

This candidate never sets can-fly, flying, gravity, movement speed, coordinates, or movement bitfields directly. Server LAND/GLOCK may request a native queued flight exit; the candidate does not grant flight in response to TAKEOFF/ANIM/impulse. It waits for ordinary server/native flying and can-fly state before movement assistance. Server flight-rate packets remain authoritative for actual speed. The donor already retains impulse vectors diagnostically rather than integrating Surge/Whirling coordinates; that behavior is preserved.

The Win32 donor also contains raw coordinate collision correction, manual stall descent, manual surge lift, airborne flying/gravity restoration, forced ground flag rewriting, and manually clearing backward/ascending flags. These are intentionally not copied into a native physics bypass. Native collision/movement remains authoritative. Native backwards input remains native backwards movement while BRK is sent; exact donor braking/coasting response needs a matching-server/native-movement review. Stall descent and surge lift physical parity remain unsupported until a typed server-approved native contract is identified. Landing detection relies on ordinary native/server movement rather than raw coordinate settle/probe heuristics or fabricated LAND notifications.

The donor ignores a MODE0 inside a transient ability/takeoff window. This candidate preserves that window but records an unpaired revocation deadline: if no matching MODE1 arrives, it resets the controller after the transition ends. The change prevents the controller staying enabled indefinitely after a server revocation.

Advanced animation support is determined from actual mounted native models. Missing sequences are skipped or use the donor's explicit fallback; no guessed model names or sequence aliases are introduced. Spell availability, costs, cooldowns, flight permission, speed and takeoff remain server-owned.

## Thread and lifecycle boundaries

Transport callbacks only validate size and queue bounded owned bytes; they never call Lua, send packets, mutate movement or reenter the protocol. The queue is capped at 64 and fails closed by clearing pending state on overflow. Packets carry transport session plus captured map; the movement thread discards stale session/map payloads. Reset callbacks request owner-thread teardown. Current player GUID, map, mounted-model identity and model handle scope the session; dismount/model replacement clears ownership and requests server resynchronization.

No actor/model references are retained. Every model mutation resolves the current typed active player and native mounted model. Animation replacement only suppresses ordinary all-bones updates for the current authorized native owner; forceful native operations and other actors/bones keep their ordinary paths. Releasing input ownership does not stop a new player's movement or cancel a user's currently held forward/backward input.

## Integration hunks

The patch contains ten candidate files. The two owned files can be copied after hash verification. The remaining eight files are review snapshots with narrow hunks; do not overwrite newer parallel edits with complete snapshots.

| Native seam | Change |
|---|---|
| `WXL/CMakeLists.txt` | Default-ON `WXL_ENABLE_SKYRIDING`, compile definition, `WxlSkyriding`/`WXL::Skyriding` object module and facade collection |
| NativeFeatures / ModuleApi | Register `wxlSkyriding` CVar and report module state/dependencies |
| ClientHelpers | Install exclusive native handlers before legacy packets; destroy on UI/transport teardown |
| LegacyPackets | Remove 527/529 outgoing and 528/52A incoming from legacy declarations so the compiled owner remains exclusive |
| `MovementIdleMoveUnits` | Tick immediately before ordinary MoveUnits; typed movement requests enter the normal event/collision/status path |
| `CGWorldFrame` | Handle space/X after normal UI focus handling; release tracked keys before key-up dispatch |
| `CGUnit_C::SetKeyBoneSequence` | Suppress ordinary all-bones updates only while the exact native Skyriding animation owner is valid; the owner itself uses force=1 |

The module remains compiled even with its option OFF so its explicit seams link; `Enabled()` gates global native/protocol/client-extension flags and the independent CVar. The independent CVar follows existing creation-effects registration style and requires no new shared bit allocation.

## Required acceptance and rollback

Root must build the exact merged source, bind outputs to the final source hash and retain pending status. Gameplay gates include matching mod-skyriding protocol, all native mounted models/sequences, ordinary flight grants/revocations, two-stage skyward, full whirling, reverse dive exit, stall and braking limitations, rapid dismount/remount, map/instance/transport changes, session reconnect, UI rebuild, CVar disable during owned input/animation, foreground/editbox input handling, and long-frame/native movement event queue behavior. Confirm the legacy facade cannot send or observe the four exclusive IDs. Native impulse ACK handling requires matching-server verification, not an inferred protocol extension.

Rollback is source-only: remove the two owned files and inverse only the integration hunks. No installed or remotely published artifact exists. Include this stage, manifest and unsupported physics parity list in the combined pending release-control handoff; do not claim the module is accepted because it compiles.
