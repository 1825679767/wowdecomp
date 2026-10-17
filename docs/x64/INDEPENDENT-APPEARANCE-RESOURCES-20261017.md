# Independent appearance resources

Compared against the current dirty WXL CharacterGeosets.cpp and RetailEquipment.cpp donor. Whole eye/hair/decoration textures bind independently there; a failed attachment does not undo successful attachments.

The native compositor formerly returned failure for the entire body/clothing recipe when a separate whole-slot image failed. It now retains the body atlas and queues each selected whole material independently, with two loads per invocation and three attempts per material, one second apart. Readiness remains incomplete if a selected material is unavailable. Original replaceable handles are retained before first successful publication and restored on recipe changes/retirement. Newly loaded whole slots are replayed to already owned customization children. No FDID, race mapping, geoset or shader is guessed.

A transient failed body build was previously cached permanently for its signature. It now receives at most three attempts. The existing component queue retains pending work after classic composition without recomposing the classic sheet while waiting. No additional worker or global owner registry is introduced.

Equipment formerly destroyed the complete candidate slot when any selected model/texture/remap failed. Successful pieces now retain exact model/attachment identities across retries. Missing pieces are retried at most ten times, 500ms apart; unavailable resources remain incomplete, not accepted. Metadata publication and changed lower-slot collection ownership invalidate the appropriate children. The native DBC-first boundary, exact material paths, bone-remap validation and creation-reference ownership are preserved. No blanket display-ID override.

Build once as the combined native WXL/DX11 Release family. Broad user eye/gear/race visual and gameplay acceptance remain pending; these fixes do not prove every screenshot has been corrected. FPS limits remain optional and existing live Unlimited settings/addon are untouched. No new client or Data copies, archive changes or game/account actions.
