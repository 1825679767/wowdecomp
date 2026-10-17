# User appearance regressions: 17 October 2026

The first broad user test of source db5435a exposed appearance failures. Prior
source/compile completion is not visual parity. Four supplied screenshots show
missing Human arm/hand regions, incorrect Tauren clothing/body regions, a bald
or incomplete female actor beside a supplemental UI, and classic Goblin controls.
The user also reports Earthen/Haranir icons selecting the original race actors.

## Source repairs

- Primary actor routing: creation and selection previously always constructed
  the old DBC display model. RetailChoices simultaneously required that old
  path to equal the DB2 recipe, preventing the two replacement wire slots from
  acquiring their actual model/customization authority. Resolve the mounted,
  MD21-validated recipe before constructing the actor. Keep private/classic
  fallback and the actual-instance path check. Apply the same routing to a
  player's primary default race display in-world; NPCs, transformations and
  alternate displays retain native model ownership. Earthen/Haranir keep the
  donor's explicit Dwarf/Night Elf equipment family fallback. No DBC row edit.
- Modern geometry: native initialization starts every section visible while
  classic GeosRenderPrep resets only IDs 0..2000. Reuse the donor policy for
  unclaimed modern groups, modern hair, base ID0 and HeadSwap fallback. The
  recipe still owns customization IDs and native equipment still owns its
  unclaimed classic groups. This is a confirmed parity omission; attribution
  of the exact photographed missing arm sections remains a visual test.
- Clothing atlas: follow each native CreateRequest region's layer limits,
  active m_layerMask and hidden-pants policy instead of repainting every cached
  handle. Only the eight actual equipment body sections participate; modern
  face/detail layers retain recipe ownership.
- Customization UI: integrate the donor CharacterCustomize.cpp tabbed Glue
  bridge with native typed option/choice/swatches/identity/generation setters.
  Replace the temporary bottom-right creation panel; retain its barber use.
  All choices remain owned by the existing component/session/form APIs, with
  exact server save gating. Native setters avoid donor x86 double-cycles and
  composition pulses. A palette click is one validated exact-choice mutation.
  Private race counters come from their actual native catalog, not guessed UI
  indices. Form switching retains the existing alternate actor owner.

## Validation and next acceptance

Both integrated Lua chunks compile with LuaJIT (Lua 5.1 language compatibility).
The combined Release build and exact frozen source identity check are recorded
with the resulting pending candidate. No user credentials, account actions or
gameplay tests are performed by this repair. The user's running game is never
replaced. All original screenshots, failed evidence and older runtimes remain.

Check Human/DK arms and hands; Tauren chest/pants/boots; gear hide/show; Goblin,
Earthen and Haranir models, both sexes and tabbed named/swatches choices;
creation-to-selection-to-world and saved extra choices. These are acceptance
gates, not implied successes. Existing unrelated donor/new-retail limitations
remain separate. DX11/environment behavior is not changed by these repairs.
