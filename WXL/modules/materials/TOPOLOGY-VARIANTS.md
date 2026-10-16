# Custom mesh full-topology ownership

The custom source21/33/35 native GPU recipe can now follow a palette partition. Preparation captures a bounded byte-index source palette; this is not GPU admission. Before publication, BindTopology validates the actual final device-sized palettes, bone references, vertex windows, contiguous normalized weights and every material row.

The unchanged serializer builds a private material-only canonical template from the exact prepared source batches. Each rebuilt native row must match its origin byte-for-byte except the section index. A new immutable Source preserves its parent source/audit/fingerprints, retains the native-origin map, duplicates the appropriate source batch records and captures each final section's actual palette. ConvertedMatches and the normal native draw checks still run. All LODs remain model-owned and the original archive SKIN read must still match before replacement. No global alias or generic shader policy is widened.

Exact DnD and named Rift paths retain their separate contracts. CPU topology composition remains unsupported until owned CPU vertex/index captures can be regenerated for the rewritten sections. Normal CPU-skinned material support is integrated separately. Joined geometry and the actual GPU register budget remain bounded.

This is code integration, not visual acceptance. DX11, native world/fog/liquid rendering and the Win32 runtime family are untouched.
