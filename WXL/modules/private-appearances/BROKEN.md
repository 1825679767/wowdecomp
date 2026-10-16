# Native Broken male/female appearance integration

Frozen source candidate based on native source HEAD `730373071586086e21d52858841bd2a73a4ac62b`. Status: pending source integration, unbuilt, uninstalled, no gameplay/visual acceptance. This stage contains five complete candidate files, a unified integration patch, and `manifest.json` with independent SHA-256/size records for all changed source inputs, donor inputs, and candidate outputs. No shared source files, runtime files, server data, build output, or release selections were changed.

## Integration

Compare each of the first five manifest input hashes with the current native file before copying its matching candidate. If a file changed, merge only its patch hunks. All five files must be integrated together. Existing private-appearances feature/CVar/build wiring and post-compositor lifecycle hooks are reused; no new CMake or module registration change is required.

Changed files:

- `WXL/modules/private-appearances/PrivateAppearances.cpp`: actual Broken profile identity, validation, mounted resources, native body synchronization, post-compositor geometry and equipment policy, readiness and restoration.
- `WXL/PrivateAppearances.hpp`: authored private option default choice.
- `WXL/modules/appearances/RetailChoices.cpp`: exact profile defaults/options, swatches, native private hair synchronization, and separate retail-axis predicate.
- `WXL/RetailChoices.hpp`: typed private/native-axis bridge declarations.
- `WoW/Common/ComponentCore/CharacterComponent.cpp`: classic next/previous/random axis fallback and validated native hair-color synchronization.

`stage.py` is the source-only generator/provenance aid. Do not rerun it after integration against changed source; preserve this frozen candidate and its before hashes.

## Authored profile behavior

Wire race 23, sex 0 and 1 require the actual sex-matched mounted native Broken model path, independently of normal retail DB2 recipes. Existing ChrRaces/CreatureDisplayInfo/CreatureModelData authority checks remain in RetailChoices. A disguise actor cannot acquire this private profile through the normal native component path.

Both sexes use all ten donor palette textures `character\\broken\\hair00_00.blp` through `hair00_09.blp` in replaceable slot 6. Exact donor swatch RGB values are preserved. Female horn styles select 201, 202, or 203; default is 202. The explicit profile default is placed first in the existing default-choice mechanism, so default 202 is also serialized/restored correctly. Female crest preserves a genuine native selected 1502..1506, defaults to 1502, suppresses incomplete 1501, and enables insert 302. The insert's slot 2 aliases the current native composited body slot 1 after the compositor finishes; it never constructs or substitutes a body atlas. Slots 402..404 remain under native control.

The classic skin, face, hair style, hair color, and facial/tendril axes remain owned and validated by the mounted native CharSections data. The former generic retail-ready guards intercepted private actors and returned failed retail operations; the separate predicate now lets those actors reach their real classic setters/cyclers/randomizers. Broken normalization also retains native component validation. Private palette choices synchronize through the validated native hair setter, and native hair changes update the private choice revision. Preparation remains blocked until that classic body rebuild has finished.

## Geometry and native equipment authority

The donor's authored baseline picks the lowest existing section per modern group 20..59 and the first existing head section while preserving native body ID 0. Female horns and crest are applied afterward. The final desired mask incorporates exact native ItemDisplayInfo/HelmetGeosetVisData seven-group helmet masks using the same native equipment-race bit convention and placeholder sections. A real native cape display remasks its overlapping 1500 group. Other equipment groups preserve the native component's current mask. Broken is explicitly excluded from the donor retail helmet policy, so no retail DB2 helmet substitutions are enabled here.

Visibility is written only where the final value differs, followed by one OptimizeVisibleGeometry when needed. This avoids repeatedly re-showing sections hidden by a helmet. The post path never recursively invokes GeosRenderPrep.

## Ownership and transport

The existing component-owned private state keeps native component serial, model pointer/handle, choice revision, retained textures, and original replaceable bindings. Actual mounted texture paths, load status, model slots, required sections, and GPU texture readiness must all pass before readiness. Female body alias originals are retained even when the original slot is null and restored on release. Native preferences, item displays, and native geoset slots form a readiness signature, so changes invalidate prepare readiness until post-compositor binding catches up.

Existing appearance-session v2 creation prepare/catalog/one-use ACK and authenticated snapshot application use the expanded narrow private identity/validation/readiness path. No new packet/opcode, account action, blind PrivateModel wire enablement, or server bypass is introduced. Broken male selections require exactly the palette row; female selections require exactly palette and horn rows. Alternate selections and out-of-range/duplicate/extra rows fail closed. Tuskarr/Murloc profiles retain their previous source behavior.

## Remaining gates and explicit limits

The parent performs one combined build after source integration. No build or tests were run in this stage. Mounted Broken models, hair textures, classic CharSections, native helmet/cape records, both-sex UI interaction, authenticated creation/snapshot round trips, body rebuild timing, equipment changes, cancel/reset, actor retirement, and visual output still require runtime acceptance. Missing native resources or an invalid classic palette/body combination intentionally leave private readiness false.

Authenticated in-world extra-choice barber saving remains unsupported by the existing server protocol and is still fail-closed from the earlier barber stage. This source port does not add that server feature. Naga and unrelated private profiles are outside this stage. No DX11 work, install, packaging, release promotion, or publication is included.
