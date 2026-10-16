Integrated native source; source/build binding and acceptance tracked separately.

# Native barber retail-detail preview candidate

Pending source integration, unbuilt. No shared source changes, build/test/gameplay runs, installation, account action or publication were performed. The owned files are `candidate/WXL/BarberAppearance.hpp` and `candidate/WXL/modules/barber-preview/BarberAppearance.cpp`; ten other candidate files are integration review snapshots. Merge their hunks against current source instead of overwriting parallel edits. The manifest captures exact source input bytes and observed HEAD; no binary source binding is claimed.

## Implemented flow

The native server barber-open path starts a scoped owner for the actual active player's component. Editing remains unavailable until the existing authenticated appearance-session hello/catalog and queried snapshot have completed, the snapshot's complete option/choice set equals the current component's actual model authority, and native appearance rendering reports ready. It supports the actual primary or alternate model family currently displayed; it never constructs or switches an alternate world actor.

The existing retail-detail controls then appear under `BarberShopFrame`, using the same native option/choice APIs and actual mounted labels. Only extra axes are editable there. Stock barber hair/color/facial/skin controls keep their native paths; the extra UI cannot mutate those legacy axes. The UI says previews cannot be saved and the ordinary Reset control restores the exact saved extra rows.

Native Apply is intercepted before its unchanged-check and packet construction. If extra choices still equal the authenticated baseline, ordinary stock barber edits remain allowed. Existing saved server extras do not by themselves block Apply. If the user has a local extra-choice delta, Apply fails closed with a specific message directing them to Reset those details before applying stock changes. No fabricated selection opcode or native character/account action is sent.

Reset and cancel restore the exact saved extra option/choice rows transactionally while preserving the legacy axes selected by the native reset/player-byte path. This distinction avoids reverting an accepted ordinary barber haircut when its native success flow closes the panel. Restoring uses complete current authority validation; unknown, duplicate, missing or incompatible rows fail without partial changes. The current actor's render dirty state is updated through the existing native choice application path.

The scoped preview suppresses background server-snapshot reapplication for only that exact component while editing; ordinary world actors and other players continue refreshing. It uses session token, active GUID, component serial, model pointer/handle, race/sex and actual recipe family. Display/native-display mismatch, non-normal component flags or mismatched player identity prevent all appearance mutation. If a disguise temporarily makes rollback unsafe, a bounded single deferred rollback waits for the same session/player/component serial and validated normal model authority to return. It is discarded across identity/session changes. UI/transport teardown ends the preview before the existing protocol owner is destroyed.

## Why saving is blocked

This was verified from the actual matching server source, not inferred from opcode names:

- `E:/6.0_EU_WOW_CORE_ACUPDATE/src/server/game/Handlers/AppearanceHandler.cpp`, `HandleWxlAppearancePrepare`, accepts prepare only when `!GetPlayer()`.
- The existing v2 prepare wire contains a Glue character name plus selection; it has no barber request GUID/chair/cost/commit operation.
- The donor `extensions/wxl-modern-m2/src/compat/ServerAppearance.cpp` likewise requires a nonempty character name and supplies the Glue prepare flow.

The existing create/customize prepare/ACK/one-use-permit code is unchanged. This candidate adds a read-only authenticated snapshot accessor and uses the existing scoped snapshot recovery path. It sends no appearance prepare for a logged-in player and does not introduce a server save protocol. Persistent extra-detail barber support remains an explicit matching-server prerequisite, including barber-chair permission, actor/form identity, price, prepare binding, ACK/commit lifetime and atomic save/broadcast behavior.

## Integration

| File/seam | Change |
|---|---|
| `WXL/CMakeLists.txt` | Default-ON `WXL_ENABLE_BARBER_PREVIEW` and compile definition; private source included in the existing WxlAppearances object module |
| NativeFeatures / ModuleApi | Independent `wxlBarberPreview` CVar and module metadata |
| RetailChoices API | Exact-authority extra-option classification, delta comparison and transactional extra-only restoration |
| RetailChoices Lua | Selects the scoped barber editor while barber is active, rejects classic-axis editing through retail-detail APIs, resets bridge on Lua initialization |
| RetailChoiceUi | Barber parent, extra-only choices, saved-detail preview note, no alternate-form switch while barber is active |
| AppearanceSession | Read-only authenticated snapshot copy; exact current barber editor excluded from automatic snapshot reapplication |
| Native BarberShop | Begin after enable, restore/end before disable, reset restoration, fail-closed Apply delta gate |
| ClientHelpers teardown | End before transport owner destruction |

The owner-thread Lua ticker is created only when barber starts; it validates scope and refreshes at 200ms. It does not create a recurring Codex automation. Public declarations are C++17-compatible; private source is C++20 through the existing object module target. Disabling the CVar ends and restores a current valid preview on the next tick.

## Exact unfinished cases

- Extra-detail persistence is unsupported by the existing matching server's authenticated prepare contract and is intentionally blocked.
- Private synthetic profiles remain unsupported here: Broken hair slot 6 and female horn geosets 201..203, Tuskarr female slots 11..13 and Murloc female slots 11/31 need their own native post-classic-compositor/profile authority integration. Their donor paths were inspected but no speculative recipe replacement was added.
- No alternate world actor construction, private-race enablement, account customization action, new native barber cost or native style-ID mapping was introduced.
- The existing server's classic-barber handling versus persisted full modern selections still needs a matching-server merge/broadcast audit. This candidate preserves the native stock send path and does not claim that the existing server rewrites modern legacy-axis rows after a classic haircut.
- Actual FrameXML placement, native shader/render/texture readiness, extra UI behavior, saved overrides, stock haircut success, Reset/cancel, morph/disguise, UI reload, session loss and actor replacement are gameplay acceptance gates. Source inspection and a successful combined build do not establish visual or persistence acceptance.

Rollback is source-only: remove the two owned files and inverse only these integration hunks, preserving unrelated dirty changes. Include this stage and its manifest in the combined pending release-control handoff. Nothing has been installed, packaged, accepted or remotely published.
