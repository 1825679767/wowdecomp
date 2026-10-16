Merged source integration; combined build and gameplay acceptance are recorded separately. Original staging contract follows.

# Native challenge-mode source candidate, 2026-10-16

Pending source-only E-drive stage. No shared source changes, client build, tests, game launch, SQL application, runtime install, packaging, acceptance promotion, or publication were performed by this task. The input native repository began at adbe55c82110b9a3211c388563a50482dbcc8f40. `candidate/` is a source-root overlay; merge wiring hunks with concurrent native module work rather than overwriting it. `baseline/` retains the exact eight changed source inputs. `source-inputs.json` records individual baseline, donor, and typed API hashes; `candidate-outputs.json` records output sizes/hashes; `challenge-mode.patch` is the source-relative patch. No binary source-binding claim is made.

## Implemented donor breadth

`ChallengeMode.cpp` ports the actual donor packet parsers and complete native Lua surface rather than a synthetic feature stub. Snapshot protocol v1/v2, boss-info v1-v3, keystone v1/v2, completion v1/v2 including member rating data, history v1/v2, loot-specialization state, and bounded reward toasts retain donor validation and wire layouts. The donor bootstrap retains its full `wxlwow.challenge_mode`, `C_ChallengeMode`, `C_MythicPlus`, scaling, affix/map, history/rating, loot-selection, keystone, and action compatibility behavior. Development keystone, reroll, abandon, accept-upgrade and start requests remain server requests with the donor action/capability bytes; no gameplay outcome is asserted locally. The original request throttle and Lua result-stack reservation remain.

Eleven declarations commit atomically through `protocol::RegisterNativeDispatch` before raw legacy packet registration:

| Direction | Opcodes |
| --- | --- |
| Client request/actions | 0x52b, 0x52d, 0x531, 0x533 |
| Server state/info/toast | 0x52c, 0x52e, 0x52f, 0x530, 0x532, 0x534, 0x535 |

The same eleven IDs are removed from `LegacyPackets.cpp`; existing transport `CanLegacySend/CanLegacyObserve` also deny compiled owners. No raw observer drains these packets. Existing quest, radial, spell-charge, appearance, and paragon owners remain intact. NativeProtocol::Capture checks IsOwner before dispatching handlers. Receivers only parse and publish value state while that transport lock exists; donor direct Lua executions were replaced with dirty flags. The donor 100ms UI poll calls the native notification pump after root FrameXML, outside transport/snapshot locks. Session generation, active player GUID and map changes reset all stored state/toasts/request throttles. Reset revisions advance so the existing Lua pull logic sees empty replacement data. No native object pointer is saved in packet state.

## Difficulty and native source seams

The original independently enabled `WXL_CHALLENGE_MODE` and `WXL_MYTHIC_DIFFICULTY` defaults map to default-on `WXL_ENABLE_CHALLENGE_MODE` / `WXL_ENABLE_MYTHIC_DIFFICULTY` build options and `wxlChallengeMode` / `wxlMythicDifficulty` runtime CVars. Changes require UI/world reload as stated by the CVar help. Both depend on the existing client-extension facade; only the challenge transport additionally depends on custom protocols. Public `ChallengeMode.hpp` remains C++17-compatible; the private object target uses C++20.

`Script_SetDungeonDifficulty` accepts dormant zero-based mode 2 when the Mythic toggle is enabled. All existing native leader checks, difficulty-state handling, and standard packet sending remain in that function. `CGGameObject_C::GetModelFileNameInternal` delegates to the typed selector before display lookup for authored challenge actor entries, then applies the donor game-object-only purple portal fallbacks. It retains Font of Power entry 5001004, countdown barrier 1000003, reward chest entries 5001100-5001102, and the exact donor authored paths.

`CGGameObject_C_Type_DungeonDifficulty::UpdateDisplay` keeps native raid visibility logic and adds the donor nonraid Mythic heroic-skull exception. Destination Map.dbc identity, not the player's outdoor map, classifies the portal. Normal/heroic/mythic model selection uses typed game-object properties 87/88. The same seam refreshes resident type-31 portal models through `CM2Scene::CreateModel`, native `SetModel`, and `CM2Model::Release`. Tracking pairs GUID, selected path, and observed model pointer, clears on player/map change, is bounded, and adopts newly streamed residents before any live swap. A recursion guard prevents SetModel callbacks from reentering model replacement. No global model/path interception, static doodad rewriting, absolute addresses, memory patches, or binary hooks are present. The donor's final disabled static-legacy-portal policy is preserved.

## Boundaries and remaining gates

This module is Mythic+ transport and presentation plus difficulty/portal compatibility. It does not implement or claim Hardcore/Challenge permanent-death enforcement, resurrection vetoes, login rejection, character persistence, quest eligibility, dungeon scaling, timer authority, loot awards, or keystone authorization. Those remain server responsibilities. No server source or SQL is modified. The existing rendered Challenges UI and parser-facing catalogs are untouched.

The authored models and their dependencies must already exist in the matching native asset family. This stage records source paths, not a newly assembled asset archive. Do not install individual Win32 DLLs or package/update MPQs or catalogs as part of this source overlay. Full native compilation, linking, matched-asset loading, popup behavior, portal visibility/model refresh, map/logout/UI reload transitions, all supported packet versions, history/rating/toasts, and server interoperability remain unverified. The root integration build and gameplay acceptance must retain their own exact source/build evidence. A successful compile alone cannot establish server enforcement or gameplay parity.

Rollback before integration is to leave this candidate unused. After integration, remove only the new module files and reverse the exact reviewed source hunks, preserving all unrelated dirty work. No reset or clean is appropriate.
