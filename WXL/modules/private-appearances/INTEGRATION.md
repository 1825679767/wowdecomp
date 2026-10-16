Integrated source candidate; compiled/runtime identity recorded separately.

# Native private creature appearance profiles

Pending source integration, unbuilt. This E-only stage contains two owned source files and eight shared integration snapshots plus `integration.patch` and exact input/output hashes. No shared source, server source, account state, runtime package or installed files were modified. No builds, tests or gameplay were run. Root integrates the shared hunks and performs its combined build. No DX11 work is included.

## Complete authored subset

| Existing wire identity | Private authority | Exact donor appearance data |
|---|---|---|
| Race 14, sex 1 | `PrivateModel(14,1)`, one `PrivateSkin` option with choices 1..3 | Tuskarr female creature skins `creature\\tuskarr\\tuskarrfemale_hd_4039141.blp`, `_4039142.blp`, `_4039147.blp`; replaceable slots 11, 12, 13; native section IDs 0, 301, 1101 and 1102 |
| Race 28, sex 1 | `PrivateModel(28,1)`, `PrivateSkin` and `PrivateOutfit`, each choices 1..5 | `creature\\murloc2maria\\murloc2_skin_%02u.blp` in slot 11 and `murloc2_outfit_%02u.blp` in slot 31, zero-based texture suffix 00..04; native section ID 0 preserved; authored fixed type-0 textures remain native |

Those wire identities come from the actual shared v2 protocol and donor customization code. The donor calls race14 female Tuskarr; no female-Naga identity is guessed or substituted. Murloc skin/outfit names and swatch colors are copied from the donor. Tuskarr choices retain actual display variant identities 1, 2 and 7.

The native mounted ChrRaces Ã¢â€ â€™ CreatureDisplayInfo Ã¢â€ â€™ CreatureModelData path remains authoritative. The private authority accepts only the donor's actual model stem families and requires the mounted native model file. `RetailChoices::For` then requires that exact canonical native path to equal the actual loaded model path. No replacement actor is created and no modern playable ChrModel is borrowed for these creatures.

The private profile descriptors do not query retail DB2 schemas or borrow a male playable recipe. Their feature is independently default ON (`WXL_ENABLE_PRIVATE_APPEARANCES`, CVar `wxlPrivateAppearances`) and uses native model/texture support. Existing authenticated appearance saving still requires its existing appearance-save/protocol feature gates.

## Typed post-compositor path

`PrivateAppearances.cpp` uses real `CCharacterComponent`, `CM2Model`, `M2SkinProfile`, `TextureCreate`, `TextureGetGxTex`, `ReplaceTexture` and `HandleDuplicate`/`HandleClose`. Native component/model pointers and handles supply ownership identity. A per-component opaque state holds exact replacement resource references and rollback originals; component destruction releases it before the model is retired.

Owner-thread post guards run after native `GeosRenderPrep`, `RenderPrepSections`, `ProcessFinishedRequest` and public `RenderPrep` returns. The normal classic body/equipment compositor keeps its ownership. Private profiles run afterward so ordinary texture/geoset rebuilding cannot silently erase their selected creature skins. Pending worker requests are not touched. A public RenderPrep post guard retries bounded owned resources after asynchronous readiness instead of introducing a render hook or fixed ABI.

The donor fallback geometry policy is preserved: one head in IDs1..99 and the lowest authored section in each modern group20..59, without changing native stock groups. Both creature base pieces under section ID0 remain visible. Tuskarr explicitly restores authored companion body sections301/1101/1102. Missing required native sections or replacement slot declarations fail closed. This candidate does not invent geoset IDs from model names.

All selected resources must exist through mounted SFile resolution. Texture handles must preserve the exact canonical requested name and must not carry a native error status. Every resource and required native section is validated before the profile state is published; actual Gx texture readiness is required before binding is reported ready. Missing/pending/incompatible resources leave the profile unready and block a prepared character save. A previous visual may remain while a new selection is unresolved; it is never treated as the newly selected profile.

Native replacement textures retain their own references. A choice change captures or carries forward the exact original slot references, including original-null bindings. Feature disable restores those native slots and marks the component for native recomposition. No model reference or account object is retained by the private state.

## Authenticated create and snapshot path

The integration does not remove private-model gates generically. Only these two identities pass `private_appearances::Validate`, which requires the exact complete bounded option set, valid 1-based choice IDs and no alternate rows. Broken identities continue failing that validation.

Private choices use the existing native RetailChoices owner for UI changes, complete option capture, revision tracking, transfer and rollback. Their options are independent extra axes; native legacy bytes are not normalized modulo a synthetic count, and native stock axis handlers are not intercepted by these profile options. Default private choices still require the authenticated appearance protocol, so an untouched default profile is not silently created without its server selection.

Before a Glue prepare, the private profile must be actually renderer-ready for the current component/model/choice revision. The existing hello/catalog check, one-use prepare permit, exact captured character context, server ACK, revalidation and typed native create/customize dispatch remain intact. No server opcode, account action or permission is added.

Incoming private snapshots must pass the same narrow profile validator. The world/selection refresh owner then chooses these profiles' primary private rows, transactionally applies the existing native choice state, and lets the post-compositor bind the resources. Actor identity and native path matching remain required. The server continues to validate every selection against its authenticated catalog and remains the persistence authority.

## Shared integration files

- `RetailChoices.cpp`: private exact-path authority, extra option metadata/labels/swatches, unchanged classic-axis fallback and protocol requirement for private defaults.
- `AppearanceCompat.cpp`: these two identities skip normal retail DB2 composition; `AppearanceReady` consults the actual private state. Also includes the root-requested narrow skinned-child teardown hunk: disable emitters/ribbons before dropping skinning source/reference.
- `AppearanceSession.cpp`: narrowly gated private ready/validation/primary-row restore support; no wire or ACK changes.
- `CharacterComponent.h/.cpp`: opaque per-component owner and native post-pass/destruction lifecycle.
- CMake, NativeFeatures and ModuleApi: independent feature/CVar and module metadata; source joins the existing WxlAppearances object module.

The shared files are review snapshots, not permission to overwrite newer parallel changes. Merge patch hunks after checking the manifest's per-input hashes. Public declarations remain C++17-compatible; private source uses the existing C++20 module target.

## Exact remaining cases

- Broken male/female remain unsupported and fail closed in this native protocol extension. Their full donor contract includes palette slot6, female horns201..203, body-sheet alias slot2, insert302, paired crest1502..1506 and post-customization equipment/helmet masking. A palette-only helper would not implement that full profile safely.
- No female-Naga private identity exists in the inspected shared protocol allowlist. Do not alias one to Tuskarr or use a retail playable recipe as a substitute.
- Existing barber extra-choice saving remains blocked because the matching server prepare handler only accepts Glue sessions. This candidate does not expand the barber transport contract or construct alternate world actors.
- Native model/texture archives, exact server catalog availability and actual visual/equipment behavior are unverified. The code skips rather than guesses absent assets/slots/required sections. Model-path acceptance does not establish model completeness.
- Required gameplay gates: both private identities, every skin/outfit combination, native body/gear/equipment attachments, rapid race/sex/class changes, create/ACK/source context changes, existing-character reload, world/other-player snapshots, logout/relog, component/model retirement, feature disable/re-enable, missing/corrupt textures and resource lifetime under native async composition. Compile success is not visual or persistence acceptance.

Rollback is source-only: remove the two owned files and inverse only these listed integration hunks. Preserve dirty unrelated work. Root must include this source stage and its unresolved gates in the combined pending release-control handoff. No installed, packaged, accepted or remotely published state is claimed.

Root integration note: post-compositor geometry now compares the exact intended native per-section mask before changing visibility; unchanged frames avoid Unoptimize/Optimize reallocation. Texture refs rebind only when native composition replaced the slot. Feature definition is private to WxlAppearances to avoid unrelated module rebuilds.
