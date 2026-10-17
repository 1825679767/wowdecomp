# Private preview actors, forms and loading - 2026-10-17

User5ddd7a4 feedback: general visuals look okay, with slow invisible initial actors/timeline running, misplaced stock panel form control, missing female Tuskarr/Murloc creature actors, white Broken hair/tendrils and residual Hide Gear body clothing. This is scoped user feedback, not full gameplay/all-race acceptance.

Private texture binding had the same extension-bearing .blp versus native extensionless cache identity mismatch as equipment; it now uses native TextureIsSame and retains/completes the native read before validation. Broken type6 palette and female private textures retain exact authored paths/options; no guessed tint or texture substitution.

Private actor selection previously returned the old DBC path before current x32 HdSwitch redirects. Exact female14 path character/tuskarr/tuskarrfemale_hd.m2 and female28 creature/murloc2maria/murloc2maria.m2 now feed both actor construction and private option authority, only when mounted. Other models stay unchanged; actual source/path/profile checks retained. HdSwitch current dirty source added to build binding.

Timeline start now waits while Glue outfit loader hides an incomplete model. This does not claim cold conversion is faster. Alternate owner retains a valid initial model while bounded async appearance work completes instead of discarding it and permanently suppressing the same attempt. Existing central model form switch stays owner-validated; its circular artwork comes from the mounted Customize control, rooted in central view and anchored below it. Legacy secondary-model preview/button is retired.

Hide Gear additionally retires residual item-owned compositor layers across dynamic sleeve/robe priority slots and queues actual recomposition, retaining skin/underwear and base/private choices. Existing slot/hand/effect cleanup retained.

One combined Release WowClient/XvidDecoder build with exact source freeze/postcheck; no client/game/account or tiny fixture loop, no broad diagnostics. Actual private female models/textures, Broken hair/tendrils and Hide Gear, form switch placement and central switching, cold visible animation start remain pending visual confirmation. C-test launcher only with rollback; E-live,Data,MPQs,Interface,WTF,Win32/Waystone/config untouched. FPS optional. No client/Data/Downloads copies or deletion.
