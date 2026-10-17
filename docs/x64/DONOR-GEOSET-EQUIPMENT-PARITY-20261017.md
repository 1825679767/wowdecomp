# Donor geometry and equipment parity correction

Current dirty donor CharacterGeosets.cpp, RetailEquipment.cpp and RetailSkinProvider.cpp are the source authority for these corrections. Prior source-contract completion did not establish visual parity. The last reported black eyes/actors and incorrect gear remain failed evidence until a new actual user result identifies a fixed case.

## Integrated source omissions

- SelectionFor in the donor merges each selected option's independent geosets with the full settled related-choice recipe. The native compositor previously kept only the full recipe. Native now merges both using the same selected option/choice pairs.
- A selected type19 eye material restores opaque iris/sclera3301 only when no explicit group33 choice is present. This donor rule was entirely missing in native and can otherwise leave an empty/black socket despite a correctly loaded texture.
- Female Orc Eyesight explicitly owns5101/5102/5103 and selects Both/Right/Left by the actual choice name. The native port now carries that rule, including Neither leaving every cornea variant hidden.
- Void Elf male owns actual root group24 horn sections; None hides them. The default-empty7760207 child is suppressed unless Horns/Blindfold/Tattoo has a non-None selection. Actual model path and selected named options delimit this donor rule.
- Native equipment had added ModelEntry.geoFilter as a mandatory second filter, although current donor RetailEquipment never consumes it. Native now follows RetailSkinProvider: rigid attachments retain their authored geometry, while root collections retain only actual named SKIN groups owned by the inventory slot. No arbitrary source section is enabled outside that slot.
- Donor retail helmet hide-group rules also apply to native DBC helmets. Native now preserves their exact DBC visibility ID (including zero) and reads retail rules for it; colliding DB2 displays never replace the native ID. Rules apply only after the current native helmet is loaded and its native identity check passes. Existing owned visibility restoration remains in place.
- A retail option with more than256 choices remains a full explicit option/choice-ID selection rather than invalidating the entire race for not fitting a classic preference byte. Legacy wire fields remain bounded; server/visual acceptance is separate.
- Native ready-state checks require actual loaded model/skin/texture owners before composing.

## Comparison boundaries

Current actor/race mapping, native whole texture loading, body rectangle/priority composition, exact material paths, native DBC precedence, source-bound palette remaps, strict geometry/material gates and lifetime ownership remain. This is not permission to infer new shaders, high-bone support, class roster policy or server formats. Donor approximations remain approximations.

Seven exact appearance table members in explicit C-test and E-live patch-z archives are byte-identical: ChrModel, ChrCustomizationChoice, ChrCustomizationElement, ChrCustomizationMaterial, CharComponentTextureLayouts, CharComponentTextureSections and TextureFileData. Evidence is E:/WXL1.1/build/donor-appearance-data-witness-20261017/report.json;8.17MiB scoped extracted members, no full client/Data copy. This does not prove every model/texture or mounted archive override matches. User clarified C is the test stage and DevAPI packages feed E-live; neither path is replaced by inference.

Build once as the combined x64/DX11 family and keep optional FPS/config unchanged. Remaining acceptance: actual eyes/gear/Earthen/Harranir/other races, selection/world/save and uncapped memory on the exact installed output. No source comparison or compilation implies1:1 rendered parity. No additional confirmed gap is invented to fill time while waiting for actual runtime evidence.
