# Native appearance session stage

Source-only stage against native source baseline `f7c7695`, with live native
component lifetime/model identity dependencies supplied by root. No tracked
checkout files were edited by this stage. No builds, tests, sends, accounts,
installation or acceptance occurred here.

Integrate the new `WXL/AppearanceSession.hpp` and
`WXL/modules/appearance-session/AppearanceSession.cpp` into the native appearance
object target (or its own compiled native object target). Both shared donor
headers are included verbatim under `WXL/vendor/include/wxl`.

The staged NativeProtocol files add atomic grouped compiled packet handler
registration. All three outbound declarations and three inbound handlers commit
together, or none do. The compiled handler receives a copied native packet view
and transport session generation at the existing owner-side capture boundary.
The callback may parse/store state only; no Lua calls, transport reentry or sends
occur while the transport lock exists. Session reset callbacks invalidate its
state. Those opcodes never enter the competing generic raw-pop queue. Appearance
outbound declarations are also hidden from generic Lua raw sends.

Root will adapt public RegisterNativeDispatch from the staged `std::span`
signature to pointer/count for C++17 native headers; the implementation itself
remains compiled as C++20. This is a required integration adjustment, not a
runtime compatibility claim.

Staged `CharacterCreation.cpp` demonstrates the required source seams:

1. Replace the old blanket extra-ID preview-only guard.
2. Call `appearance_session::BeforeCreate(name)` **after native name/default-name
   validation and immediately before `CHARACTER_CREATE_INFO` construction**.
3. Call `appearance_session::Tick()` from creation Idle, outside transport locks.
4. Call `appearance_session::Cancel()` at creation OnHide before removing Idle.

Classic/default-extra choices keep the standard native byte-field path. Explicit
extra choices queue one bounded command; the initial native call returns. Idle
performs hello negotiation, sends the exact selection prepare, then waits for a
server `status == 0` reply with the exact current monotonically unique request
ID. No callback sends or resumes creation. Idle re-reads and compares the current
component, monotonic `m_wxlSerial`, root model pointer/`m_handle`, profile bytes,
class, existing-character index/GUID/customize flags, choice revision, exact
option/choice pairs, name and transport-session generation. Only then does it
arm a single-use permit and invoke typed native `CreateCharacter` outside locks.
That function validates again and consumes the permit before its existing
create/customize/race-change/faction-change path. Duplicate replies cannot reuse
the permit. No acknowledgement is synthesized.

Hello and prepare use the exact shared version-2 grammar, current catalog ID
3060710526, and race-map version 20260907. A full primary selection is built by
matching `CollectRetailChoices` against every current AppearanceStore option
and its choice membership. At most 64 choices are accepted. The single command
expires after 10 seconds; model/context/session mismatch, unsupported alternate
or synthetic data, rejection and send failure refuse or cancel with a dialog.
OnHide clears queued work. Requests are never retried indefinitely.

The name is the immutable, already validated native CreateCharacter command
argument, and is passed through native validation again on resume. Editing a UI
text field without submitting another create command does not inherently mutate
that native argument. If the intended UI policy cancels such queued intent when
typing, wire `Cancel` to the name field's change event. A new CreateCharacter
command with another name replaces pending work and cannot reuse its reply.

`CanPrepareCurrent()` performs structural supported-family/session validation
without fake names, sends or acknowledgements. Root can use it for
`WXL_RetailSelectionCanSave` when explicit extra IDs are present; its true result
means the prepare path is available, not that the server has prepared a save.

`QuerySelectedCharacter()` obtains the existing GUID through typed
CharacterSelection display APIs. Hello negotiation also starts independently
from `RefreshCharacter`, so restoration does not depend on a queued create.
Negotiation permits three attempts separated by at least three seconds;
incompatible replies fail closed until session reset. Each GUID permits three
query attempts with the same three-second throttle. Session reset clears these
bounded counters. Query replies enter a session-scoped cache only for requested
GUIDs, with monotonically checked snapshot revisions. Cache/query/application
memo maps each hold at most 512 GUIDs.

Root must call `RefreshCharacter(guid, component)` in selected-character Idle
using its current display GUID/component, and in GameUI Idle using the existing
native active-player GUID/component already available there. This stage does not
guess an object manager or resolve arbitrary world units. It copies a matching
catalog, primary-form, race/sex snapshot, verifies `RetailChoicesReady` against
the native component/model, then calls root's transactional
`ApplyRetailChoices(component, RetailChoicePair*, count)` outside every state
and transport lock. Only that per-component authority applies the complete
explicit selection and corresponding classic-axis preferences. Shared model
resources are not mutated. Creation's live preview owner is excluded.

Application attempts are memoized by GUID/server revision/current session/
component pointer/lifetime serial/root/model handle. The same snapshot does not
overwrite live per-component edits every frame; a replacement component/root or
newer server revision permits a new application. Failed authority applications
are memoized too rather than repeated every frame.

Wire `InvalidateCharacter(guid)` to a successful existing-character
customize/race-change/faction-change result. It clears that GUID's cache, query
retry state and application memo, allowing a fresh snapshot query. It is
deliberately not called merely when submitting the save, because an immediate
query could observe the old server revision before commit. `HasSnapshot` still
reports cache presence/revision, without implying server save acceptance.

**Alternate forms, synthetic models, barber state are not implemented by this stage.** Player/selection restoration and
creation/save ordering are source work; actual server persistence and visual
results require separate acceptance.

Donor references: x64-waystone worktree
`include/wxl/AppearanceProtocol.hpp`, `AppearanceCatalogVersion.hpp`, and
`extensions/wxl-modern-m2/src/compat/ServerAppearance.cpp`. No binary host/module
injection, fixed addresses, auth evidence or gameplay acceptance is implied.

Root integration adds a dedicated WxlAppearanceSession target and wxlAppearanceSave CVar (default ON), a C++17-compatible pointer/count dispatch API, transactional choice application, selection/active-player idle refresh and successful native character-update invalidation. No source/runtime acceptance implied by staging.


Broader source batch also calls RefreshCharacter from typed player UpdateDisplay, sharing a four-query/second budget and bounded cached/retry state. Root identity validation excludes unsupported/NPC/transformed model families before querying. Caches refresh no faster than sixty seconds after a received snapshot, expire actors after120 seconds idle and prune at most once/second. Active barber edits suspend source refresh. The existing snapshot revision/model/lifetime memo avoids repeatedly overwriting local live edits. No new barber wire grammar is invented.
