# Native topology and donor material recipe composition

E-only source candidate against HEAD78b19e1385c100bb37d695a8bc5392187dd0d52d. Exact seven prior files are in before/, complete candidates in review/. Source status/diff and source/input hashes are supplied. No shared source writes, build, tests, fixture work, runtime, DX11, installation or publication occurred.

## Concrete integrated path

Native full-geometry palette partitioning previously refused every model-owned donor fallback recipe because splitting duplicated/reindexed its batches. This stage composes those two existing policies while preserving their exact material contract and full geometry.

1. skin::Owned gains batchOrigins. SkinRebuild appends the original prepared-SKIN batch index for every emitted duplicate, in the same loop/order as its24-byte batch row.
2. ConvertForNativeRuntime gains a default-null native-only optional per-profile converted-batch origin output. Each original prepared source batch receives its canonical converted-template index. Each rebuilt batch emits that origin, plus origin+1 when the existing EnvSplit conversion emits a follower. The output map therefore names original converted template rows, not raw source rows. Map counts are checked against the actual converted SKIN count, and outputs publish only after successful complete model conversion. Generic Convert, service/partition policy, default calls and all geometry/palette limits remain unchanged.
3. AssetCompat requests paired native SKINs/origins. For a donor-owned profile, it serializes an exact original prepared batch template with the unchanged ConvertSkin function. The private carrier contains only the48-byte SKIN header plus exact prepared24-byte source batches, with every geometry array empty. It is never admitted, published or consumed as geometry. This avoids rejecting a material-only comparison just because the original geometry exceeds75 bones; actual rebuilt geometry has already passed full device-budget Skin/ConvertSkin validation.
4. Every actual rebuilt native batch must equal its mapped converted template byte-for-byte, ignoring ONLY the two-byte section index at+4. Shader, flags, priority, source color, material, layer, texture count/combo, UV combo, weight and transform indices must all match. No transform/bone/UV threshold is relaxed.
5. A new immutable DonorMaterialRecipe clone retains its parent recipe, exact source audit/fingerprints, canonical template rows and complete output origin map. Only nativeBatches is changed to the actual output count; each original secondaryUV fix is copied to every duplicate naming that original converted row. Donor collapse/default-diffuse recipes with no UV fix use the same verified path. All named LOD profiles compose independently.
6. FinishDonorMaterialSkin revalidates all mapped24-byte rows (again ignoring only section index), even when there are no UV fixes, then applies its existing exact UV1 contract checks and patch. The exact original archive SKIN comparison and native model-owned publication/lifetime remain unchanged.

## Auditing and boundaries

Original DonorMaterialBatch.firstNativeBatch remains the canonical converted-template index. Logs now label it canonical-template-batch, avoiding a false actual-draw-index claim. The cloned nativeBatchOrigins vector maps actual rebuilt output index back to that canonical record; topologyParent preserves the unmodified original native count/fixes/audit. Original raw model and SKIN ownership remain in ModelAssets.

Exact custom mesh recipes (including weighted source21/33/35 and DnD) remain refused when topology is rebuilt; their original section/palette/source-index identity is not reused. This stage adds no custom shader, renderer path, fixture exception, fallback equation, partition limit, discarded geometry or generic policy admission. Existing native triangle streams, section draw order, multi-LOD vertex/combo joining and device palette bounds remain authoritative.

## Integration

Apply integration.patch or copy the seven complete review files. The only API extension is the default-null final ConvertForNativeRuntime origin argument; callers not requesting it retain their existing behavior. The extra batchOrigins ownership field is populated by all Rebuild calls without changing their serialized output.
