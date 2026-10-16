# Native component TextureCache ownership and palette conversion

E-only source stage. Captured native HEAD6b1838b392fa722f0c07475e65fdecde407575ff plus concurrent dirty source status/diff; exact four input files are independently captured under before/ and hashed in manifest.json. No shared source writes, build, tests, runtime, renderer queue changes, DX11 changes, installation, or publication.

## Actual component path

The native async CACHEENTRY route reads BLP bytes directly and does not call the sized CBLPFile::Source/ConvertBlp path. Native component Paste expects palette indices plus separate alpha; DXT bytes previously reached its green fallback. The old20-bit m_size allocated a truncated buffer while CAsyncObject retained the complete SFile size.

- CACHEENTRY filename capacity is260 bytes; m_size is a full uint32. ObjectAllocAddHeap already uses sizeof(CACHEENTRY). Filename copies and alternate names use typed sizeof, and TextureCacheCreateTexture refuses empty/nonterminated/over-capacity paths before hashing or allocation. Failed entry/object/buffer allocations return cleanly.
- LoadTexture obtains both SFileGetFileSize words before allocation. It rejects high-word files, sizes below the bounded BLP header, and files above64MiB, closes files on every pre-owner failure, and gives SMem allocation and CAsyncObject exactly the same full size. The original file name is retained for the separate full-quality atlas ReadImage path.
- Existing async ownership/cancel/FreeRequest behavior remains. Postload first destroys its completed CAsyncObject (closing its file), then invokes the bounded component bridge. Only then can an SMem-owned replacement be allocated/copied and the original buffer freed. Failure frees the retired-owner buffer once and publishes terminal failure with zero info. Destructor cancellation still passes the original async buffer to FreeRequest exactly as before.
- WXL PrepareComponentBlp validates BLP2 header and palette/mip extents. Valid classic paletted bytes stay completely unchanged. Modern DXT uses the existing TextureComponentToPaletted(...,256). BGRA encoding3 first uses existing TranscodeBlp, then the same component palette conversion. Converted component images are checked to remain <=256px on their long edge. BlpEnabled gates modern conversion; classic palette validation works independently.
- Header checks enforce supported alpha depths, native-compatible power-of-two dimensions<=4096, required palette bytes, contiguous available mip entries, per-level required color/alpha/block bytes, in-buffer offsets/sizes, no overlap, and no extra levels beyond the1x1 chain. Modern input must contain the mip needed for the256px budget; the donor decoder's fallback to a larger base mip is refused. Classic partial chains publish their actual contiguous valid count, preserving file bytes while avoiding fabricated absent mips.
- Native info is published only after validation/conversion. GetMip/GetPal/HasMips refuse pending/unvalidated buffers; GetMip also rechecks its stored offset/size. HasMips remains nonblocking. Explicit GetInfo(wait) retains the existing queue join behavior. GetInfo initializes output on every path and retains the native terminal-failure completion semantics (return1 with zero dimensions), so a failed texture does not wait forever. Existing ComponentItemsLoaded(0) readiness/retry remains authoritative.

## Bounded limitation

The existing CAsyncObject callback ABI exposes neither bytes-read nor final read-success after its retry loop. The allocation is zero-initialized and all published structure extents are validated, so missing bytes cannot expose uninitialized memory. This stage does not modify the source queue or invent a success flag; it cannot distinguish a structurally valid zero-filled partial payload from an equally shaped complete image. Non-BLP/JPEG/unsupported encoding, malformed or over-budget input, and modern files without a usable<=256 mip fail closed.

## Integration

Apply integration.patch or copy the four complete review files. Component body-layer code already uses sizeof(cached->m_fileName), so recompilation naturally consumes260-byte capacity. The component-specific palettization never mutates the archive path or the independent full-quality ReadImage/atlas decoder.
