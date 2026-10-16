# Native external animation publication bounds

The DX11 integration rc3 handoff independently reported that FinishLoadingLowPrioritySequence accepted a one-byte declared external animation allocation for actual Ogre tracks. Native code previously ignored the stream size and relocated selected leaf offsets unconditionally. This is a common Model2 loader issue, independent of the graphics backend; the reported race crash is not established as caused or fixed by this correction.

Before native publication, M2ExternalSequence validates the relocated model metadata arrays and each selected non-global time/key leaf against the declared external allocation size using widened/subtraction-based byte bounds. Coverage follows native M2Init: bones, colors, texture weights/transforms, attachments, events, lights, cameras, ribbons and particle emission/visibility tracks. Native inline particle lifetime tracks and global-loop tracks stay in the original model allocation. Global-loop tracks are no longer re-relocated through external sequence initialization.

The async entry validates the complete bounded alias ring before changing any leaf offset or ready flag. Invalid/out-of-range/non-closing rings refuse. Already-loaded indices remain idempotent. Null metadata/buffer and reentrant native relocation refuse. A rejected async completion retires its normal playback-load owner without starting primary/secondary bone playback from invalid data. The allocation remains under the existing native sequence-buffer owner until ordinary teardown, avoiding borrowed/free mismatches.

Classic and modern animation bytes, IDs, interpolation and file naming are unchanged. No synthetic animation or visual fallback is introduced. Bounds validation is not proof of every source family, short-read transport status, Ogre appearance, or race creation acceptance. The existing native SFile async reader remains responsible for its read-success contract. DX11 rendering is not edited here.

## Combined DX11/native integration

The final source merges DX11 df64c66 transactional leaf-write planning with the native ba75caa read-only model-table preflight. Each alias is model/stream validated before a single deduplicated write plan commits. The plan additionally checks signed32 relative pointer representability, handles shared leaf headers once, restores temporary native sequence context with RAII and bounds file-request alias traversal. Rejected playback stays suppressed under the existing pending-clip policy. Source compatibility is separate from new combined-binary gameplay acceptance.
