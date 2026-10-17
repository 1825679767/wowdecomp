# Broad-test appearance/material repairs

This change follows the second user screenshot set. The previous runtime was
only partially improved and is not visually accepted. The donor is the actual
dirty WXL tree, not merely its HEAD or an old exported DLL.

## Integrated corrections

- Reuse the donor's existing compact MaterialIndex, including its ordered
  texture-type hint, UsageType 2, first-row fallback. The former native map
  required an exact usage and discarded duplicate material/usage rows. Both
  body and independent eye/hair/detail sheets now pass their actual texture
  type instead of always requesting usage zero.
- Bind equipment materials using their complete TextureFilePath authority.
  Do not rebuild an unrelated ObjectComponents path from its basename. Rows
  targeting replaceable types absent from this particular child are ignored,
  as in the donor, rather than rejecting and retiring the whole slot.
- Select the race recipe's actor before construction independently of the
  old DBC actor and classic customization-axis limits. Cache successful exact
  mounted paths only. Recipe actors already converted to native MD20 v264
  are supported as well as modern MD21 containers. The normal native loader
  still validates the full model and companions before consumption.
- Recipe composition requires the actual loaded model path to match its DB2
  identity; it no longer depends on having run an in-process MD21 conversion.
  Preconverted models receive their recipe-owned eyes/body/details too.

Private appearances, NPC ownership, server save validation, race mapping,
source-held model lifetime and DX11 renderer behavior are retained. No archive
or data update is inferred or installed. C and E data remain separate.

## Acceptance

One consolidated Release build and frozen source checks are required. That
does not establish pixel correctness. The supplied Human eyes/3D equipment,
Tauren clothing, replacement Earthen/Haranir actors and creation gear must be
checked on the resulting exact runtime. Missing mounted assets or unmatched
schemas remain data issues, not permission to invent a replacement texture.
The separate VSync/memory regression remains open; FPS 60/30 is mitigation.

No new client or Data copies, automatic account actions or repeated native
fixture loops are part of this change.
