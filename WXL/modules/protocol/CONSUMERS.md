# Portable native opcode consumers

`ProtocolConsumers.cpp` adapts the existing WXL area-loot, garrison and
spell-charge modules into compiled native source. It uses direct Lua, FrameScript
and transport calls. It contains no module host, executable offsets or binary
hook adapter.

The source transport invokes consumers only at the native owner-side
`ProcessMessage` boundary, after verifying the current session. Parsing and state
publication never call Lua while the transport or native event queue holds a
lock. Existing addon APIs consume their dedicated state; generic custom pop no
longer competes for these three builtin opcodes.

| Feature | Native Lua API | Wire and bounds |
| --- | --- | --- |
| Area loot | `_WXLWOW_AREA_LOOT_SEND`, `_WXLWOW_AREA_LOOT_POP`, `_WXLWOW_AREA_LOOT_CLEAR` | `0x550` request: 1-64 printable ASCII bytes. `0x551` state: 1-8192 ASCII bytes split into nonempty newline-separated records of at most 240 bytes. Queue: 256 records, donor oldest-record eviction. |
| Garrison | `_WXL_GARRISON_SEND`, `_WXL_GARRISON_POP`, `_WXL_GARRISON_CLEAR` | `0x552` request: at most 254 bytes. `0x553` state: at most 4096 bytes. Required 1-48 byte uppercase/underscore command and tab; body permits tabs and UTF-8 bytes, rejects other controls and DEL. Queue: 1024 records, donor queue clearing on overflow. |
| Spell charges | `wxlwow.request_spell_charges`, `wxlwow.spell_charge_snapshot`, `wxlwow.spell_charge_row` | Empty `0x525` request, 250ms throttle. `0x526`: version 1, uint16 little-endian count, exact 18-byte rows: uint32 spell/category, uint8 maximum/current, uint32 duration/remaining milliseconds. Current clamps to maximum; zero spell/maximum rows are skipped. |

All feature functions also have their donor `WXL64_*` aliases. Spell data functions
additionally retain `_WXLWOW_REQUEST_SPELL_CHARGES`,
`_WXLWOW_SPELL_CHARGE_SNAPSHOT`, and `_WXLWOW_SPELL_CHARGE_ROW`. The existing
transport cap of 10,235 bytes also applies to spell snapshots, permitting at most
568 wire rows. The broader donor parser's 64KiB ceiling does not enlarge this
transport limit.

Snapshot info returns count and revision. A row returns spell, category, maximum,
current, duration and elapsed-adjusted remaining milliseconds. Parsing validates
the complete packet before publishing replacement rows or any text record. State
is bounded and session-scoped. Disconnect, UI context reset, feature disable
observed by the data APIs, and an observed current-client/transport change clear
stored records and spell state. Saved owner pointers are compared, never
dereferenced. An allocation failure clears feature state rather than publishing
a partial snapshot.

`protocol::UiReady()` must run after root FrameXML loading. It executes the donor
spell-charge presentation bootstrap with a required-GameUI-function guard. The
bootstrap preserves the existing action button presentation, request events and
0.1-second revision polling. Glue still receives the data APIs but skips action
wrappers. Repeat root initialization retains the named native functions so later
GameUI loading can install its presentation. Area-loot and garrison use their
existing addon polling and clear calls. The server retains all gameplay authority.

Add `ProtocolConsumers.cpp` to WXL CMake sources. Register the addon defaults
`wxlAreaLootAuto=1` and `wxlAreaLootQuality=0` in NativeFeatures. No new compiler
definition is needed. No automatic hello/request is sent by initialization.

Donor source worktree:
`C:/Users/blanc/.codex/worktrees/x64-waystone/WXL1.1`:

- `extensions/wxl-area-loot/src/Module.cpp`
- `extensions/wxl-garrison/src/Module.cpp`
- `extensions/wxl-spell-charges/src/SpellCharges.cpp`

Appearance save is not implemented. Its coherent catalog negotiation,
hello/prepare/result/query and character create/barber flow remains a separate
integration requirement; no appearance acknowledgement is fabricated.

This source change has not been built, tested, installed or exercised against an
account or server. Native-source compilation, packet fixtures and live UI/gameplay
acceptance remain separate gates.
