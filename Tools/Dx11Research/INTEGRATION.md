# Combined native x64 / DX11 integration

Branch `integration/dx11-x64-20261016` starts from captured x64 commit
`a6e243bb257d00241c22aa48cf522c7326560964`, preserves its captured uncommitted
custom-material visibility ownership fix, and merges all eight DX11 commits
through `c524e6a55360cf90e61e275ce2686c60d3b9a162`.
The subsequent merge includes x64 `f4f8382bd124d9cc92d934a106f800647cba30b0`:
native CPU custom material draws, rebuilt topology/section ownership, procedural
enchant composition, timed phases and clone lifetimes. CPU state restoration
also uses the typed backend dispatcher on DX11.

The active x64 and original DX11 repositories remain independent. Later active
x64 changes require a reviewed subsequent merge; this branch names an exact
captured source point rather than claiming to track a continuously moving tree.

New WXL material/particle modules previously admitted only D3D9/9Ex. Their
existing source contracts and native shader programs now also admit the DX11
converter. Backend synchronization explicitly handles each supported API;
DX11 draws rebind native Gx state without D3D9 casts. Native PBNT2 retains
UINT byte palette indices and normalized byte weights.

DnD's exact native program/blend/depth guard uses Gx-owned shader bytes and
state on DX11. The stage2 linear/clamp sampler override is local to the draw,
does not change a shared texture's flags, and restores the previous override.
Blood Boil captures current RT0 with MSAA resolution, not the backbuffer;
captures belong to the DX11 device, resize transactionally and are retired on
device recreation/destruction. The original source/geometry/shader guards,
RGB-only write mask, constants and draw-local state restore remain required.

`NativeDx11BackendTests` includes current WXL programs, actual paired shader
draw submission, rigid RGB pixels, weighted palette-index/weight pixels,
layered custom strides, DnD programs, stage-local sampler state, scene capture
pixels and donor refraction. These automated proofs do not establish visual
or authenticated world acceptance. Retain D3D9/default-OFF builds and private
offline fixtures; no active-client installation or publication is implied.
The optional `fullscreen` backend test also exercises exclusive mode entry,
focus loss/return, gamma application/restoration paths and return to a window.
GPU target pixels and successful API transitions do not establish monitor
colour calibration, visible gamma correctness or performance acceptance.
