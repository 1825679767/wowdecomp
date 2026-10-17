# Remaining race children and VSync-off resource ownership

This follows the user's second appearance report and request to fix uncapped
memory growth. It does not assume the previous candidate was visually accepted.

## Race/gear source corrections

Current dirty WXL RetailEquipment groups customization records by child FDID,
clears non-base child geometry, restores only authored selected geosets and
maps bones by canonical keys, the authored key lookup, CRC names and parents.
The native port instead deduplicated by FDID, enabled every child variant and
required exact bind-pose/flags topology. One refused child invalidated the
whole body atlas. These are concrete mismatches affecting separate Earthen
crystals/hair and Harranir quills/hair.

The native implementation now keeps the exact recipe groups and independent
child owners, with a typed Customization skinning mode. Equipment retains its
own bind-pivot/geometry-scale correction policy. The explicit authored key-bone
lookup is restored before CRC/parent fallback. Invalid scenes, cycles, camera
authority and ambiguous names still refuse unsafe mapping.

Child loading progresses through the existing main-thread component queue,
at most two groups per call, with finite retry/time budgets. A missing child
does not restore the old body sheet or remove eyes and gear. Full appearance
readiness remains false until all requested children are ready; exhausted
failures do not spin. Child/source references retire with the component.

## DX11 changes

- Two persistent event queries retire the oldest frame before its slot is
  reused. Normal rendering permits at most two submitted frames, retains one
  for overlap, and gxFixLag drains immediately. This applies with VSync and
  CPU FPS limits off. GPU failure/timeout stops new submissions.
- Compare complete per-stage constant-buffer payloads before uploading.
  Always rebind the buffers; shader inline constants and fixed-function values
  remain part of that comparison. Device recovery clears snapshots.
- Nonindexed fans share a canonical immutable index buffer and use the native
  base-vertex offset. Indexed fans reuse one bounded dynamic buffer. Buffers
  grow geometrically to bounded capacities and retire with the device.

Microsoft documents that UpdateSubresource under contention uses temporary
command-buffer storage and a later GPU copy. Eliminating identical uploads and
bounding frame submission addresses that concrete churn/backlog mechanism; it
does not by itself prove every cause of the reported live-game memory spike.
References: https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-updatesubresource
and https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-getdata .

## Validation scope

One combined Release build, frozen native/donor source checks, and one bounded
native hardware frame-resource regression: VSync/fixLag/FPS limits off,
240 frames/15360 draws, indexed/nonindexed fan pixels, unchanged constant
uploads, stable resource owners/private bytes, immediate drain and recovery.
This check is not an account/gameplay reproduction or race visual acceptance.
No new client, archive or Data copies are needed. FPS 60/30 mitigation remains
until user acceptance; the fix itself does not depend on that cap.
