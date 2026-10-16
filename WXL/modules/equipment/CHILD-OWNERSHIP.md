# Equipment child cleanup ownership

Each source-owned equipment Child retains the native creation reference while validation, texture/geoset selection and attachment can fail. Native CM2Model::DetachFromParent is not an unbound no-op: it always consumes an attachment reference. Calling it during validation failure could therefore delete the creation-only model, followed by Release on the freed pool object. An already detached child had the same hazard during later slot cleanup.

Child now stores the expected parent pointer and model handle. Its destructor stops emitters and unbinds only the matching actual attachment or skinning-source relation. An unbound or moved child is not explicitly detached; the owner then drops its one creation reference exactly once. Stored parent identity is never dereferenced unless it equals the actual current native relation. Normal successful rigid cleanup still consumes the attachment reference and creation reference separately. Source-bound skinned cleanup retains its existing typed SetSkinningSource release semantics.

The fix is from source review against native DetachFromParent, not a replayed runtime result. Broad failed-load/equip/unequip/scene-retirement acceptance remains for the combined client test. No new effect/asset admission or user-runtime probes.
