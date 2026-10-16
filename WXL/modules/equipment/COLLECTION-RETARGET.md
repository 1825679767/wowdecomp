# Equipment-only collection retargeting

SetSkinningSource keeps exact topology mapping as the default for appearance children. Equipment collections opt into a separate donor policy after exact mapping fails: canonical bone key, nonzero bone-name CRC and bounded mapped-parent fallback. Ambiguous source keys/names are not chosen by array order; unresolved helpers retain their own native pose. Both source and dependent skeletons must be finite, bounded and acyclic.

Native model references/reverse dependent lists and the existing joined animation owner retain lifetime and scheduling. A legacy source body receives the donor per-bone row-vector bind-pivot correction with canonical extent scale clamped0.55..1.45; wire18 female preserves authored scale1. Modern-loaded source bodies retain their authored pose without legacy pivot correction. Actual particles/ribbons/lights use the synchronized child bone matrices. Collection attachment metadata may be retained, but actual attached descendants and cameras do not acquire ownership through this route.

This ports the donor approximation; it is not a recovered anatomical rig. Native DBC weapons, ordinary appearance children and unrelated scene owners remain unchanged. Runtime geometry fit, clipping, animation and retirement acceptance require the broad client test.
